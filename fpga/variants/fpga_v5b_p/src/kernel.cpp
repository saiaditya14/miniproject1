#include "kernel_cfg.h"

#define KMAX 768
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
#pragma HLS PIPELINE II = 1
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

static void load_b_tile(const float *B, float Bbuf[KMAX][NT], int K, int N, int j0, int nt) {
#pragma HLS ARRAY_PARTITION variable = Bbuf complete dim = 2
    if (nt == NT && (N & 15) == 0) {
        const int N16 = N >> 4;
        const int G = NT / 16;
        int kk = 0, g = 0;
        for (int e = 0; e < K * G; e++) {
#pragma HLS PIPELINE II = 1
            const long w = (long)kk * N16 + (j0 >> 4) + g;
            for (int q = 0; q < 16; q++) {
#pragma HLS UNROLL
                Bbuf[kk][g * 16 + q] = B[w * 16 + q];
            }
            if (++g == G) { g = 0; kk++; }
        }
    } else if (nt == N) {
        int kk = 0, jj = 0;
        for (int e = 0; e < K * nt; e++) {
#pragma HLS PIPELINE II = 1
            Bbuf[kk][jj] = B[e];
            if (++jj == nt) { jj = 0; kk++; }
        }
    } else {
        for (int k = 0; k < K; k++) {
            for (int j = 0; j < nt; j++) {
#pragma HLS PIPELINE II = 1
                Bbuf[k][j] = B[(long)k * N + j0 + j];
            }
        }
    }
    if (nt < NT) {
        for (int k = 0; k < K; k++) {
#pragma HLS PIPELINE II = 1
            for (int j = 0; j < NT; j++) {
#pragma HLS UNROLL
                if (j >= nt) Bbuf[k][j] = 0.0f;
            }
        }
    }
}

static void tl_load(const float *A, float At[RT][KMAX], int i0, int M, int K16) {
#pragma HLS ARRAY_PARTITION variable = At complete dim = 1
#pragma HLS ARRAY_RESHAPE variable = At cyclic factor = 16 dim = 2
    const int mr = (M - i0 < RT) ? (M - i0) : RT;
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

static void tl_compute(const float At[RT][KMAX], const float Bbuf[KMAX][NT], float Ct[RT][NT], int K) {
#pragma HLS ARRAY_PARTITION variable = At complete dim = 1
#pragma HLS ARRAY_RESHAPE variable = At cyclic factor = 16 dim = 2
#pragma HLS ARRAY_PARTITION variable = Bbuf complete dim = 2
#pragma HLS ARRAY_PARTITION variable = Ct complete dim = 0
    float sh[RT][NT][LACC];
#if ACC_STYLE == 2
#pragma HLS ARRAY_PARTITION variable = sh complete dim = 1
#pragma HLS ARRAY_PARTITION variable = sh complete dim = 2
#pragma HLS BIND_STORAGE variable = sh type = ram_2p impl = lutram
    float tmpv[LACC][RT][NT];
#pragma HLS ARRAY_PARTITION variable = tmpv complete dim = 0
#else
#pragma HLS ARRAY_PARTITION variable = sh complete dim = 0
#endif
    float awv[RT][KU];
#pragma HLS ARRAY_PARTITION variable = awv complete dim = 0

#if ACC_STYLE == 2
    for (int x = 0; x < LACC; x++) {
#pragma HLS PIPELINE II = 1
        for (int r = 0; r < RT; r++) {
#pragma HLS UNROLL
            for (int j = 0; j < NT; j++) {
#pragma HLS UNROLL
                sh[r][j][x] = 0.0f;
            }
        }
    }
#else
    for (int r = 0; r < RT; r++) {
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
    for (int r = 0; r < RT; r++) {
#pragma HLS UNROLL
        for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
            awv[r][q] = 0.0f;
        }
    }

    int ph = 0;
    for (int t = 0; t < K; t++) {
#pragma HLS PIPELINE II = 1
#if ACC_STYLE >= 1
#pragma HLS DEPENDENCE variable = sh type = inter direction = RAW distance = LACC dependent = true
#endif
        const int l = t & 15;
        float w[RT][KU];
#pragma HLS ARRAY_PARTITION variable = w complete dim = 0
        for (int r = 0; r < RT; r++) {
#pragma HLS UNROLL
            for (int q = 0; q < KU; q++) {
#pragma HLS UNROLL
                w[r][q] = (l == 0) ? At[r][(t >> 4) * 16 + q] : awv[r][q];
            }
        }
        for (int r = 0; r < RT; r++) {
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
        for (int r = 0; r < RT; r++) {
#pragma HLS UNROLL
            const float a = w[r][0];
            for (int j = 0; j < NT; j++) {
#pragma HLS UNROLL
#if ACC_STYLE >= 1
                sh[r][j][ph] += a * b[j];
#else
                float cur = sh[r][j][LACC - 1] + a * b[j];
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

#if ACC_STYLE == 2
    for (int x = 0; x < LACC; x++) {
#pragma HLS PIPELINE II = 1
        for (int r = 0; r < RT; r++) {
#pragma HLS UNROLL
            for (int j = 0; j < NT; j++) {
#pragma HLS UNROLL
                tmpv[x][r][j] = sh[r][j][x];
            }
        }
    }
    for (int r = 0; r < RT; r++) {
#pragma HLS UNROLL
        for (int j = 0; j < NT; j++) {
#pragma HLS UNROLL
            float v[LACC];
            for (int x = 0; x < LACC; x++) {
#pragma HLS UNROLL
                v[x] = tmpv[x][r][j];
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
#else
    for (int r = 0; r < RT; r++) {
#pragma HLS UNROLL
        for (int j = 0; j < NT; j++) {
#pragma HLS UNROLL
            float s = 0.0f;
            for (int x = 0; x < LACC; x++) {
#pragma HLS UNROLL
                s += sh[r][j][x];
            }
            Ct[r][j] = s;
        }
    }
#endif
}

static void tl_store(float *C, const float Ct[RT][NT], int i0, int M, int j0, int nt, int N) {
#pragma HLS ARRAY_PARTITION variable = Ct complete dim = 0
    const int mr = (M - i0 < RT) ? (M - i0) : RT;
    if (nt == N) {
        int r = 0, j = 0;
        for (int e = 0; e < mr * nt; e++) {
#pragma HLS PIPELINE II = 1
            C[(long)i0 * N + e] = Ct[r][j];
            if (++j == nt) { j = 0; r++; }
        }
    } else {
        for (int r = 0; r < mr; r++) {
            for (int j = 0; j < nt; j++) {
#pragma HLS PIPELINE II = 1
                C[(long)(i0 + r) * N + j0 + j] = Ct[r][j];
            }
        }
    }
}

static void gemm_tiled(const float *A, const float *B, float *C, int M, int K, int N) {
    float Bbuf[KMAX][NT];
#pragma HLS ARRAY_PARTITION variable = Bbuf complete dim = 2
    const int K16 = K >> 4;

    for (int j0 = 0; j0 < N; j0 += NT) {
        const int nt = (N - j0 < NT) ? (N - j0) : NT;
        load_b_tile(B, Bbuf, K, N, j0, nt);

        for (int i0 = 0; i0 < M; i0 += RT) {
#if USE_DATAFLOW
#pragma HLS DATAFLOW
#endif
            float At[RT][KMAX];
#pragma HLS ARRAY_PARTITION variable = At complete dim = 1
#pragma HLS ARRAY_RESHAPE variable = At cyclic factor = 16 dim = 2
            float Ct[RT][NT];
#pragma HLS ARRAY_PARTITION variable = Ct complete dim = 0
            tl_load(A, At, i0, M, K16);
            tl_compute(At, Bbuf, Ct, K);
            tl_store(C, Ct, i0, M, j0, nt, N);
        }
    }
}

extern "C" void gemm(const float *A, const float *B, float *C, int M, int K, int N) {
#pragma HLS INTERFACE m_axi port = A offset = slave bundle = gmem0 depth = 18432 max_widen_bitwidth = WIDEN max_read_burst_length = BURST num_read_outstanding = OUTSTANDING
#pragma HLS INTERFACE m_axi port = B offset = slave bundle = gmem1 depth = 11520 max_widen_bitwidth = WIDEN max_read_burst_length = BURST num_read_outstanding = OUTSTANDING
#pragma HLS INTERFACE m_axi port = C offset = slave bundle = gmem2 depth = 256 max_write_burst_length = BURST num_write_outstanding = OUTSTANDING
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
