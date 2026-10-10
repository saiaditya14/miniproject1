#include "kernel_cfg.h"
#if GV_MODE == 2
#include <hls_stream.h>
#endif

#define KMAX 768
static_assert(RT % CP == 0 && (CP == 1 || CP == 2 || CP == 4 || CP == 8), "CP");
#define RTP (RT / CP)
#if RTP >= 7
#define EPII 1
#elif RTP >= 4
#define EPII 2
#elif RTP == 3
#define EPII 3
#elif RTP == 2
#define EPII 4
#else
#define EPII 7
#endif
#define KU 16

static void gemm_generic(const float *A, const float *B, float *C, int M, int K, int N) {
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++) {
            float acc = 0.0f;
            for (int k = 0; k < K; k++)
                acc += A[(long)i * K + k] * B[(long)k * N + j];
            C[(long)i * N + j] = acc;
        }
}

#if GV_MODE == 0
static void gv_load(const float *A, float At[RG][KMAX], int i0, int M, int K16) {
#pragma HLS ARRAY_PARTITION variable = At complete dim = 1
#pragma HLS ARRAY_RESHAPE variable = At cyclic factor = 16 dim = 2
    const int mr = (M - i0 < RG) ? (M - i0) : RG;
    int r = 0, c = 0;
    const long base16 = (long)i0 * K16;
    for (int t = 0; t < mr * K16; t++) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = At type = inter dependent = false
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            At[r][c * 16 + q] = A[(base16 + t) * 16 + q];
        }
        if (++c == K16) { c = 0; r++; }
    }
}

static void gv_compute(const float At[RG][KMAX], const float Bv[KMAX], float res[RG], int K16) {
#pragma HLS ARRAY_PARTITION variable = At complete dim = 1
#pragma HLS ARRAY_RESHAPE variable = At cyclic factor = 16 dim = 2
#pragma HLS ARRAY_RESHAPE variable = Bv cyclic factor = 16 dim = 1
#pragma HLS ARRAY_PARTITION variable = res complete dim = 1
    float acc[RG];
#if ACC_STYLE == 2
#pragma HLS BIND_STORAGE variable = acc type = ram_2p impl = lutram
#else
#pragma HLS ARRAY_PARTITION variable = acc complete dim = 1
#endif
    for (int x = 0; x < RG; x++) {
#pragma HLS PIPELINE II = 1
        acc[x] = 0.0f;
    }
    int r = 0, c = 0;
    for (int t = 0; t < K16 * RG; t++) {
#pragma HLS PIPELINE II = 1 style = frp
#pragma HLS DEPENDENCE variable = acc type = inter direction = RAW distance = RG dependent = true
        float p[KU];
#pragma HLS ARRAY_PARTITION variable = p complete dim = 1
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            p[q] = At[r][c * 16 + q] * Bv[c * 16 + q];
        }
        for (int s = KU / 2; s >= 1; s /= 2) {
#pragma HLS UNROLL
            for (int q = 0; q < s; q++) {
#pragma HLS UNROLL
                p[q] += p[q + s];
            }
        }
        acc[r] += p[0];
        if (++r == RG) { r = 0; c++; }
    }
    for (int x = 0; x < RG; x++) {
#pragma HLS PIPELINE II = 1
        res[x] = acc[x];
    }
}

static void gv_store(float *C, const float res[RG], int i0, int M) {
#pragma HLS ARRAY_PARTITION variable = res complete dim = 1
    const int mr = (M - i0 < RG) ? (M - i0) : RG;
    for (int r = 0; r < mr; r++) {
#pragma HLS PIPELINE II = 1
        C[i0 + r] = res[r];
    }
}

static void gemv(const float *A, const float *B, float *C, int M, int K) {
    float Bv[KMAX];
#pragma HLS ARRAY_RESHAPE variable = Bv cyclic factor = 16 dim = 1
    const int K16 = K >> 4;

    for (int c = 0; c < K16; c++) {
#pragma HLS PIPELINE II = 1
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            Bv[c * 16 + q] = B[(long)c * 16 + q];
        }
    }

    for (int i0 = 0; i0 < M; i0 += RG) {
#if USE_DATAFLOW
#pragma HLS DATAFLOW
#endif
        float At[RG][KMAX];
#pragma HLS ARRAY_PARTITION variable = At complete dim = 1
#pragma HLS ARRAY_RESHAPE variable = At cyclic factor = 16 dim = 2
        float res[RG];
#pragma HLS ARRAY_PARTITION variable = res complete dim = 1
        gv_load(A, At, i0, M, K16);
        gv_compute(At, Bv, res, K16);
        gv_store(C, res, i0, M);
    }
}
#else
#define GV_SUB 8
#define GVW (RG * (KMAX / KU))

static_assert(RG % GV_SUB == 0, "RG must be a multiple of 8");

#if GV_MODE == 2
static void gv_reader(const float *A, hls::stream<float> (&s)[KU], int nw) {
    for (int t = 0; t < nw; t++) {
#pragma HLS PIPELINE II = 1
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            s[q].write(A[t * 16 + q]);
        }
    }
}

static void gv_fill(hls::stream<float> (&s)[KU], float At[GVW][KU], int mr, int K16) {
#pragma HLS ARRAY_PARTITION variable = At complete dim = 2
    for (int t = 0; t < mr * K16; t++) {
#pragma HLS PIPELINE II = 1
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            At[t][q] = s[q].read();
        }
    }
}
#else
static void gv_fill(const float *A, float At[GVW][KU], int i0, int mr, int K16) {
#pragma HLS ARRAY_PARTITION variable = At complete dim = 2
    const int base16 = i0 * K16;
    for (int t = 0; t < mr * K16; t++) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = At type = inter dependent = false
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            At[t][q] = A[(base16 + t) * 16 + q];
        }
    }
}
#endif

static void gv_compute(const float At[GVW][KU], const float Bv[KMAX], float res[RG], int K16, int nsub) {
#pragma HLS ARRAY_PARTITION variable = At complete dim = 2
#pragma HLS ARRAY_RESHAPE variable = Bv cyclic factor = 16 dim = 1
    float acc[GV_SUB];
#pragma HLS ARRAY_PARTITION variable = acc complete dim = 1
    const int total = nsub * GV_SUB * K16;
    const int last = K16 - 1;
    const int stepc = 1 - (GV_SUB - 1) * K16;
    int r = 0, c = 0, ri = 0, a = 0;
    bool fz = true;
    bool fl = (last == 0);
    for (int t = 0; t < total; t++) {
#pragma HLS PIPELINE II = 1 style = frp
#pragma HLS DEPENDENCE variable = acc type = inter direction = RAW distance = GV_SUB dependent = true
        float p[KU];
#pragma HLS ARRAY_PARTITION variable = p complete dim = 1
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            p[q] = At[a][q] * Bv[c * 16 + q];
        }
        for (int s = KU / 2; s >= 1; s /= 2) {
#pragma HLS UNROLL
            for (int q = 0; q < s; q++) {
#pragma HLS UNROLL
                p[q] += p[q + s];
            }
        }
        const float prev = fz ? 0.0f : acc[r];
        const float sum = prev + p[0];
        acc[r] = sum;
        if (fl) {
            res[ri] = sum;
            ri++;
        }
        if (r != GV_SUB - 1) {
            r++;
            a += K16;
        } else {
            r = 0;
            if (!fl) {
                c++;
                a += stepc;
                fz = false;
                fl = (c == last);
            } else {
                c = 0;
                a += 1;
                fz = true;
                fl = (last == 0);
            }
        }
    }
}

static void gv_store(float *C, const float res[RG], int i0, int mr) {
    for (int r = 0; r < mr; r++) {
#pragma HLS PIPELINE II = 1
        C[i0 + r] = res[r];
    }
}

#if GV_MODE == 2
static void gv_engine(hls::stream<float> (&sA)[KU], const float Bv[KMAX], float *C, int M, int K16) {
#pragma HLS ARRAY_RESHAPE variable = Bv cyclic factor = 16 dim = 1
    for (int i0 = 0; i0 < M; i0 += RG) {
#pragma HLS DATAFLOW
        const int mr = (M - i0 < RG) ? (M - i0) : RG;
        const int nsub = (mr + GV_SUB - 1) / GV_SUB;
        float At[GVW][KU];
#pragma HLS ARRAY_PARTITION variable = At complete dim = 2
        float res[RG];
        gv_fill(sA, At, mr, K16);
        gv_compute(At, Bv, res, K16, nsub);
        gv_store(C, res, i0, mr);
    }
}

static void gv_stream(const float *A, const float Bv[KMAX], float *C, int M, int K16) {
#pragma HLS DATAFLOW
#pragma HLS ARRAY_RESHAPE variable = Bv cyclic factor = 16 dim = 1
    hls::stream<float> sA[KU];
#pragma HLS STREAM variable = sA depth = GV_FIFO
    gv_reader(A, sA, M * K16);
    gv_engine(sA, Bv, C, M, K16);
}
#else
static void gv_engine(const float *A, const float Bv[KMAX], float *C, int M, int K16) {
#pragma HLS ARRAY_RESHAPE variable = Bv cyclic factor = 16 dim = 1
    for (int i0 = 0; i0 < M; i0 += RG) {
#pragma HLS DATAFLOW
        const int mr = (M - i0 < RG) ? (M - i0) : RG;
        const int nsub = (mr + GV_SUB - 1) / GV_SUB;
        float At[GVW][KU];
#pragma HLS ARRAY_PARTITION variable = At complete dim = 2
        float res[RG];
        gv_fill(A, At, i0, mr, K16);
        gv_compute(At, Bv, res, K16, nsub);
        gv_store(C, res, i0, mr);
    }
}
#endif

static void gemv(const float *A, const float *B, float *C, int M, int K) {
    float Bv[KMAX];
#pragma HLS ARRAY_RESHAPE variable = Bv cyclic factor = 16 dim = 1
    const int K16 = K >> 4;

    for (int c = 0; c < K16; c++) {
#pragma HLS PIPELINE II = 1
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            Bv[c * 16 + q] = B[(long)c * 16 + q];
        }
    }

#if GV_MODE == 2
    gv_stream(A, Bv, C, M, K16);
#else
    gv_engine(A, Bv, C, M, K16);
#endif
}
#endif

static void load_b_tile(const float *B, float Bbuf[CP][KMAX][NT], int K, int N, int j0, int nt) {
#pragma HLS ARRAY_PARTITION variable = Bbuf complete dim = 1
#pragma HLS ARRAY_PARTITION variable = Bbuf complete dim = 3
    if (nt == NT && (N & 15) == 0) {
        const int N16 = N >> 4;
        const int G = NT / 16;
        int kk = 0, g = 0;
        for (int e = 0; e < K * G; e++) {
#pragma HLS PIPELINE II = 1
            const long w = (long)kk * N16 + (j0 >> 4) + g;
            for (int q = 0; q < 16; q++) {
#pragma HLS UNROLL
                const float v = B[w * 16 + q];
                for (int p = 0; p < CP; p++) {
#pragma HLS UNROLL
                    Bbuf[p][kk][g * 16 + q] = v;
                }
            }
            if (++g == G) { g = 0; kk++; }
        }
    } else if (nt == N) {
        int kk = 0, jj = 0;
        for (int e = 0; e < K * nt; e++) {
#pragma HLS PIPELINE II = 1
            const float v = B[e];
            for (int p = 0; p < CP; p++) {
#pragma HLS UNROLL
                Bbuf[p][kk][jj] = v;
            }
            if (++jj == nt) { jj = 0; kk++; }
        }
    } else {
        for (int k = 0; k < K; k++) {
            for (int j = 0; j < nt; j++) {
#pragma HLS PIPELINE II = 1
                const float v = B[(long)k * N + j0 + j];
                for (int p = 0; p < CP; p++) {
#pragma HLS UNROLL
                    Bbuf[p][k][j] = v;
                }
            }
        }
    }
    if (nt < NT) {
        for (int k = 0; k < K; k++) {
#pragma HLS PIPELINE II = 1
            for (int j = 0; j < NT; j++) {
#pragma HLS UNROLL
                if (j >= nt) {
                    for (int p = 0; p < CP; p++) {
#pragma HLS UNROLL
                        Bbuf[p][k][j] = 0.0f;
                    }
                }
            }
        }
    }
}

static void tl_load(const float *A, float At[CP][RTP][KMAX], int i0, int M, int K16) {
#pragma HLS ARRAY_PARTITION variable = At complete dim = 1
#pragma HLS ARRAY_PARTITION variable = At complete dim = 2
#pragma HLS ARRAY_RESHAPE variable = At cyclic factor = 16 dim = 3
    const int mr = (M - i0 < RT) ? (M - i0) : RT;
    int r = 0, c = 0;
    const long base16 = (long)i0 * K16;
    for (int t = 0; t < mr * K16; t++) {
#pragma HLS PIPELINE II = 1
#pragma HLS DEPENDENCE variable = At type = inter dependent = false
        float v[KU];
#pragma HLS ARRAY_PARTITION variable = v complete dim = 1
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            v[q] = A[(base16 + t) * 16 + q];
        }
        for (int rr = 0; rr < RT; rr++) {
#pragma HLS UNROLL
            if (rr == r) {
                for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
                    At[rr / RTP][rr % RTP][c * 16 + q] = v[q];
                }
            }
        }
        if (++c == K16) { c = 0; r++; }
    }
}

static void tl_compute(const float At[RTP][KMAX], const float Bbuf[KMAX][NT], float Ct[RTP][NT], int K) {
#pragma HLS ARRAY_PARTITION variable = At complete dim = 1
#pragma HLS ARRAY_RESHAPE variable = At cyclic factor = 16 dim = 2
#pragma HLS ARRAY_PARTITION variable = Bbuf complete dim = 2
#pragma HLS ARRAY_PARTITION variable = Ct complete dim = 2
    float sh[RTP][NT][LACC];
#pragma HLS ARRAY_PARTITION variable = sh complete dim = 0
    float awv[RTP][KU];
#pragma HLS ARRAY_PARTITION variable = awv complete dim = 0

#if !ZINIT
    for (int r = 0; r < RTP; r++) {
#pragma HLS UNROLL
        for (int j = 0; j < NT; j++) {
#pragma HLS UNROLL
            for (int x = 0; x < LACC; x++) {
#pragma HLS UNROLL
                sh[r][j][x] = 0.0f;
            }
        }
    }
#endif
    for (int r = 0; r < RTP; r++) {
#pragma HLS UNROLL
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            awv[r][q] = 0.0f;
        }
    }

    int ph = 0;
    for (int t = 0; t < K; t++) {
#pragma HLS PIPELINE II = 1 style = frp
#if ACC_STYLE >= 1
#pragma HLS DEPENDENCE variable = sh type = inter direction = RAW distance = LACC dependent = true
#endif
        const int l = t & 15;
        float w[RTP][KU];
#pragma HLS ARRAY_PARTITION variable = w complete dim = 0
        for (int r = 0; r < RTP; r++) {
#pragma HLS UNROLL
            for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
                w[r][q] = (l == 0) ? At[r][(t >> 4) * 16 + q] : awv[r][q];
            }
        }
        for (int r = 0; r < RTP; r++) {
#pragma HLS UNROLL
            for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
                awv[r][q] = (q < KU - 1) ? w[r][q + 1] : 0.0f;
            }
        }
        float b[NT];
#pragma HLS ARRAY_PARTITION variable = b complete dim = 1
        for (int j = 0; j < NT; j++) {
#pragma HLS UNROLL
            b[j] = Bbuf[t][j];
        }
#if ZINIT
        const bool first = (t < LACC);
#endif
        for (int r = 0; r < RTP; r++) {
#pragma HLS UNROLL
            const float a = w[r][0];
            for (int j = 0; j < NT; j++) {
#pragma HLS UNROLL
#if ACC_STYLE >= 1
                sh[r][j][ph] += a * b[j];
#else
#if ZINIT
                float cur = (first ? 0.0f : sh[r][j][LACC - 1]) + a * b[j];
#else
                float cur = sh[r][j][LACC - 1] + a * b[j];
#endif
                for (int x = LACC - 1; x > 0; x--) {
#pragma HLS UNROLL
                    sh[r][j][x] = sh[r][j][x - 1];
                }
                sh[r][j][0] = cur;
#endif
            }
        }
        if (++ph == LACC) ph = 0;
    }

    for (int r = 0; r < RTP; r++) {
#pragma HLS PIPELINE II = EPII
        for (int j = 0; j < NT; j++) {
#pragma HLS UNROLL
            float v[LACC];
#pragma HLS ARRAY_PARTITION variable = v complete dim = 1
            for (int x = 0; x < LACC; x++) {
#pragma HLS UNROLL
                v[x] = sh[r][j][x];
            }
            for (int st = LACC / 2; st >= 1; st /= 2) {
#pragma HLS UNROLL
                for (int x = 0; x < st; x++) {
#pragma HLS UNROLL
                    v[x] += v[x + st];
                }
            }
            Ct[r][j] = v[0];
        }
    }
}

static void tl_store(float *C, const float Ct[CP][RTP][NT], int i0, int M, int j0, int nt, int N) {
#pragma HLS ARRAY_PARTITION variable = Ct complete dim = 1
#pragma HLS ARRAY_PARTITION variable = Ct complete dim = 3
    const int mr = (M - i0 < RT) ? (M - i0) : RT;
    if (nt == NT && (N & 15) == 0) {
        const int N16 = N >> 4;
        const int G = NT / 16;
        long rb = (long)i0 * N16 + (j0 >> 4);
        long w = rb;
        int rp = 0, rr = 0, g = 0;
        for (int e = 0; e < mr * G; e++) {
#pragma HLS PIPELINE II = 1
            for (int q = 0; q < 16; q++) {
#pragma HLS UNROLL
                C[w * 16 + q] = Ct[rp][rr][g * 16 + q];
            }
            if (++g == G) {
                g = 0;
                if (++rr == RTP) { rr = 0; rp++; }
                rb += N16;
                w = rb;
            } else {
                w++;
            }
        }
    } else if (nt == N) {
        int r = 0, j = 0;
        for (int e = 0; e < mr * nt; e++) {
#pragma HLS PIPELINE II = 1
            C[(long)i0 * N + e] = Ct[r / RTP][r % RTP][j];
            if (++j == nt) { j = 0; r++; }
        }
    } else {
        for (int r = 0; r < mr; r++) {
            for (int j = 0; j < nt; j++) {
#pragma HLS PIPELINE II = 1
                C[(long)(i0 + r) * N + j0 + j] = Ct[r / RTP][r % RTP][j];
            }
        }
    }
}

static void gemm_tiled(const float *A, const float *B, float *C, int M, int K, int N) {
    float Bbuf[CP][KMAX][NT];
#pragma HLS ARRAY_PARTITION variable = Bbuf complete dim = 1
#pragma HLS ARRAY_PARTITION variable = Bbuf complete dim = 3
    const int K16 = K >> 4;

    for (int j0 = 0; j0 < N; j0 += NT) {
        const int nt = (N - j0 < NT) ? (N - j0) : NT;
        load_b_tile(B, Bbuf, K, N, j0, nt);

        for (int i0 = 0; i0 < M; i0 += RT) {
#if USE_DATAFLOW
#pragma HLS DATAFLOW
#endif
            float At[CP][RTP][KMAX];
#pragma HLS ARRAY_PARTITION variable = At complete dim = 1
#pragma HLS ARRAY_PARTITION variable = At complete dim = 2
#pragma HLS ARRAY_RESHAPE variable = At cyclic factor = 16 dim = 3
            float Ct[CP][RTP][NT];
#pragma HLS ARRAY_PARTITION variable = Ct complete dim = 1
#pragma HLS ARRAY_PARTITION variable = Ct complete dim = 3
            tl_load(A, At, i0, M, K16);
            tl_compute(At[0], Bbuf[0], Ct[0], K);
#if CP > 1
            tl_compute(At[1], Bbuf[1], Ct[1], K);
#endif
#if CP > 2
            tl_compute(At[2], Bbuf[2], Ct[2], K);
            tl_compute(At[3], Bbuf[3], Ct[3], K);
#endif
#if CP > 4
            tl_compute(At[4], Bbuf[4], Ct[4], K);
            tl_compute(At[5], Bbuf[5], Ct[5], K);
            tl_compute(At[6], Bbuf[6], Ct[6], K);
            tl_compute(At[7], Bbuf[7], Ct[7], K);
#endif
            tl_store(C, Ct, i0, M, j0, nt, N);
        }
    }
}

extern "C" void gemm(const float *A, const float *B, float *C, int M, int K, int N) {
#pragma HLS INTERFACE m_axi port = A offset = slave bundle = gmem0 depth = 39168 max_widen_bitwidth = WIDEN max_read_burst_length = BURST num_read_outstanding = OUTSTANDING
#pragma HLS INTERFACE m_axi port = B offset = slave bundle = gmem1 depth = 18432 max_widen_bitwidth = WIDEN max_read_burst_length = BBURST num_read_outstanding = BOUT
#pragma HLS INTERFACE m_axi port = C offset = slave bundle = gmem2 depth = 4096 max_widen_bitwidth = WIDEN max_write_burst_length = BURST num_write_outstanding = OUTSTANDING
#pragma HLS INTERFACE s_axilite port = M
#pragma HLS INTERFACE s_axilite port = K
#pragma HLS INTERFACE s_axilite port = N
#pragma HLS INTERFACE s_axilite port = return

    if ((K & 15) != 0 || K < 16 || K > KMAX) {
        gemm_generic(A, B, C, M, K, N);
    } else if (N == 1) {
        gemv(A, B, C, M, K);
    } else {
        gemm_tiled(A, B, C, M, K, N);
    }
}
