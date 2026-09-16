#include <string.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#if defined(__AVX2__) && defined(__FMA__)
#include <immintrin.h>
#define HAVE_AVX2FMA 1
#endif

#define MC 128
#define KC 256
#define NC 512
#define MR 4
#define NR 8

static void pack_A(const float *A, int K, int i0, int mc, int k0, int kc, float *Apack){
    for(int p=0; p<mc; p+=MR){
        int mr = (mc-p < MR) ? mc-p : MR;
        for(int k=0; k<kc; k++)
            for(int r=0; r<MR; r++)
                Apack[p*kc + k*MR + r] = (r<mr) ? A[(size_t)(i0+p+r)*K + (k0+k)] : 0.0f;
    }
}

static void pack_B(const float *B, int N, int k0, int kc, int j0, int nc, float *Bpack){
    for(int p=0; p<nc; p+=NR){
        int nr = (nc-p < NR) ? nc-p : NR;
        for(int k=0; k<kc; k++)
            for(int c=0; c<NR; c++)
                Bpack[p*kc + k*NR + c] = (c<nr) ? B[(size_t)(k0+k)*N + (j0+p+c)] : 0.0f;
    }
}

static inline void microkernel(int kc, const float *Ap, const float *Bp,
                                float *C, int ldc, int mr, int nr){
    float tmp[MR][NR];
#ifdef HAVE_AVX2FMA
    __m256 c0 = _mm256_setzero_ps(), c1 = _mm256_setzero_ps();
    __m256 c2 = _mm256_setzero_ps(), c3 = _mm256_setzero_ps();
    for(int k=0; k<kc; k++){
        __m256 b = _mm256_loadu_ps(&Bp[k*NR]);
        c0 = _mm256_fmadd_ps(_mm256_set1_ps(Ap[k*MR+0]), b, c0);
        c1 = _mm256_fmadd_ps(_mm256_set1_ps(Ap[k*MR+1]), b, c1);
        c2 = _mm256_fmadd_ps(_mm256_set1_ps(Ap[k*MR+2]), b, c2);
        c3 = _mm256_fmadd_ps(_mm256_set1_ps(Ap[k*MR+3]), b, c3);
    }
    _mm256_storeu_ps(tmp[0], c0);
    _mm256_storeu_ps(tmp[1], c1);
    _mm256_storeu_ps(tmp[2], c2);
    _mm256_storeu_ps(tmp[3], c3);
#else
    memset(tmp, 0, sizeof(tmp));
    for(int k=0; k<kc; k++)
        for(int r=0; r<MR; r++){
            float a = Ap[k*MR+r];
            for(int c=0; c<NR; c++)
                tmp[r][c] += a * Bp[k*NR+c];
        }
#endif
    for(int r=0; r<mr; r++)
        for(int c=0; c<nr; c++)
            C[(size_t)r*ldc + c] += tmp[r][c];
}

static void gemv(const float *A, const float *B, float *C, int M, int K){
    #ifdef _OPENMP
    #pragma omp parallel for schedule(static)
    #endif
    for(int i=0; i<M; i++){
        const float *Arow = &A[(size_t)i*K];
        int k = 0;
        float s = 0.0f;
#ifdef HAVE_AVX2FMA
        __m256 acc = _mm256_setzero_ps();
        for(; k+8<=K; k+=8){
            __m256 a = _mm256_loadu_ps(&Arow[k]);
            __m256 b = _mm256_loadu_ps(&B[k]);
            acc = _mm256_fmadd_ps(a, b, acc);
        }
        __m128 lo = _mm256_castps256_ps128(acc);
        __m128 hi = _mm256_extractf128_ps(acc, 1);
        __m128 sum4 = _mm_add_ps(lo, hi);
        __m128 sum2 = _mm_add_ps(sum4, _mm_movehl_ps(sum4, sum4));
        __m128 sum1 = _mm_add_ss(sum2, _mm_shuffle_ps(sum2, sum2, 1));
        s = _mm_cvtss_f32(sum1);
#endif
        for(; k<K; k++)
            s += Arow[k] * B[k];
        C[i] = s;
    }
}

void student_gemm(const float *A, const float *B, float *C, int M, int K, int N){
    if(N == 1){
        gemv(A, B, C, M, K);
        return;
    }

    for(int i=0; i<M; i++)
        memset(&C[(size_t)i*N], 0, (size_t)N*sizeof(float));

    for(int jc=0; jc<N; jc+=NC){
        int nc = (N-jc < NC) ? N-jc : NC;
        for(int pc=0; pc<K; pc+=KC){
            int kc = (K-pc < KC) ? K-pc : KC;

            float Bpack[NC*KC];
            pack_B(B, N, pc, kc, jc, nc, Bpack);

            #ifdef _OPENMP
            #pragma omp parallel for schedule(dynamic)
            #endif
            for(int ic=0; ic<M; ic+=MC){
                int mc = (M-ic < MC) ? M-ic : MC;
                float Apack[MC*KC];
                pack_A(A, K, ic, mc, pc, kc, Apack);

                for(int jr=0; jr<nc; jr+=NR){
                    int nr = (nc-jr < NR) ? nc-jr : NR;
                    for(int ir=0; ir<mc; ir+=MR){
                        int mr = (mc-ir < MR) ? mc-ir : MR;
                        microkernel(kc,
                                    &Apack[ir*kc],
                                    &Bpack[jr*kc],
                                    &C[(size_t)(ic+ir)*N + (jc+jr)],
                                    N, mr, nr);
                    }
                }
            }
        }
    }
}
