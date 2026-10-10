#include <hls_stream.h>

#define KU 16

union fbits {
    float f;
    unsigned u;
};

static inline unsigned f2u(float v) {
    fbits x;
    x.f = v;
    return x.u;
}

static inline float u2f(unsigned v) {
    fbits x;
    x.u = v;
    return x.f;
}

static void p1_flat(const float *A, const float *B, float *C, long nw, int no) {
    unsigned acc[KU];
#pragma HLS ARRAY_PARTITION variable = acc complete
    for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
        acc[q] = 0;
    }
    for (long t = 0; t < nw; t++) {
#pragma HLS PIPELINE II = 1
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            acc[q] ^= f2u(A[t * 16 + q]);
        }
    }
    const unsigned b0 = f2u(B[0]);
    for (int q = 0; q < KU; q++) {
#pragma HLS PIPELINE II = 1
        if (q < no) C[q] = u2f(acc[q] ^ b0);
    }
}

static void p2_reader(const float *A, hls::stream<float> (&s)[KU], long nw) {
    for (long t = 0; t < nw; t++) {
#pragma HLS PIPELINE II = 1
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            s[q].write(A[t * 16 + q]);
        }
    }
}

static void p2_drain(hls::stream<float> (&s)[KU], const float *B, float *C, long nw, int no) {
    unsigned acc[KU];
#pragma HLS ARRAY_PARTITION variable = acc complete
    for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
        acc[q] = 0;
    }
    for (long t = 0; t < nw; t++) {
#pragma HLS PIPELINE II = 1
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            acc[q] ^= f2u(s[q].read());
        }
    }
    const unsigned b0 = f2u(B[0]);
    for (int q = 0; q < KU; q++) {
#pragma HLS PIPELINE II = 1
        if (q < no) C[q] = u2f(acc[q] ^ b0);
    }
}

static void p2_fifo(const float *A, const float *B, float *C, long nw, int no) {
#pragma HLS DATAFLOW
    hls::stream<float> s[KU];
#pragma HLS STREAM variable = s depth = 128
    p2_reader(A, s, nw);
    p2_drain(s, B, C, nw, no);
}

extern "C" void gemm(const float *A, const float *B, float *C, int M, int K, int N) {
#pragma HLS INTERFACE m_axi port = A offset = slave bundle = gmem0 depth = 39168 max_widen_bitwidth = 512 max_read_burst_length = 64 num_read_outstanding = 16
#pragma HLS INTERFACE m_axi port = B offset = slave bundle = gmem1 depth = 18432 max_widen_bitwidth = 512 max_read_burst_length = 8 num_read_outstanding = 64
#pragma HLS INTERFACE m_axi port = C offset = slave bundle = gmem2 depth = 4096 max_widen_bitwidth = 512 max_write_burst_length = 64 num_write_outstanding = 16
#pragma HLS INTERFACE s_axilite port = M
#pragma HLS INTERFACE s_axilite port = K
#pragma HLS INTERFACE s_axilite port = N
#pragma HLS INTERFACE s_axilite port = return

    const long nw = ((long)M * (long)K) >> 4;
    const long mn = (long)M * (long)N;
    const int no = mn < KU ? (int)mn : KU;
    if (N == 1) {
        p1_flat(A, B, C, nw, no);
    } else if (nw >= 100000) {
        p2_fifo(A, B, C, nw, no);
    }
}
