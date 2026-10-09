#include <stdlib.h>
#include <string.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#if defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#define X86 1
#endif

#define MC 120
#define KC 256
#define NC 512
#define MR 6
#define NR 16

typedef void (*mk_fn)(int, const float *, const float *, float *, int, int, int);

static __thread float *tl_A, *tl_B;
static __thread size_t tl_Acap, tl_Bcap;

static float *get_buf(float **buf, size_t *cap, size_t need){
    if(*cap < need){
        free(*buf);
        *buf = (float *)malloc(need * sizeof(float));
        *cap = need;
    }
    return *buf;
}

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

static void mk_scalar(int kc, const float *Ap, const float *Bp,
                       float *C, int ldc, int mr, int nr){
    float tmp[MR][NR];
    memset(tmp, 0, sizeof(tmp));
    for(int k=0; k<kc; k++)
        for(int r=0; r<MR; r++){
            float a = Ap[k*MR+r];
            for(int c=0; c<NR; c++)
                tmp[r][c] += a * Bp[k*NR+c];
        }
    for(int r=0; r<mr; r++)
        for(int c=0; c<nr; c++)
            C[(size_t)r*ldc + c] += tmp[r][c];
}

static void gemv_scalar(const float *A, const float *B, float *C, int M, int K){
    #ifdef _OPENMP
    // #pragma omp parallel for schedule(static) if(M >= 2048)
    #endif
    for(int i=0; i<M; i++){
        const float *Arow = &A[(size_t)i*K];
        float s[8] = {0,0,0,0,0,0,0,0};
        int k = 0;
        for(; k+8<=K; k+=8)
            for(int l=0; l<8; l++)
                s[l] += Arow[k+l] * B[k+l];
        float t = ((s[0]+s[1])+(s[2]+s[3])) + ((s[4]+s[5])+(s[6]+s[7]));
        for(; k<K; k++)
            t += Arow[k] * B[k];
        C[i] = t;
    }
}

#ifdef X86
#define FMA2(r)     a = _mm256_broadcast_ss(Ap + (r));     c##r##0 = _mm256_fmadd_ps(a, b0, c##r##0);     c##r##1 = _mm256_fmadd_ps(a, b1, c##r##1);

#define ACC_ROW(r) { float *p = C + (size_t)(r)*ldc;     _mm256_storeu_ps(p,   _mm256_add_ps(_mm256_loadu_ps(p),   c##r##0));     _mm256_storeu_ps(p+8, _mm256_add_ps(_mm256_loadu_ps(p+8), c##r##1)); }

#define SPILL_ROW(r)     _mm256_storeu_ps(tmp[r], c##r##0); _mm256_storeu_ps(tmp[r]+8, c##r##1);

__attribute__((target("avx2,fma")))
static void mk_avx2(int kc, const float *Ap, const float *Bp,
                     float *C, int ldc, int mr, int nr){
    __m256 c00 = _mm256_setzero_ps(), c01 = _mm256_setzero_ps();
    __m256 c10 = _mm256_setzero_ps(), c11 = _mm256_setzero_ps();
    __m256 c20 = _mm256_setzero_ps(), c21 = _mm256_setzero_ps();
    __m256 c30 = _mm256_setzero_ps(), c31 = _mm256_setzero_ps();
    __m256 c40 = _mm256_setzero_ps(), c41 = _mm256_setzero_ps();
    __m256 c50 = _mm256_setzero_ps(), c51 = _mm256_setzero_ps();
    for(int k=0; k<kc; k++){
        __m256 b0 = _mm256_loadu_ps(Bp);
        __m256 b1 = _mm256_loadu_ps(Bp + 8);
        __m256 a;
        FMA2(0) FMA2(1) FMA2(2) FMA2(3) FMA2(4) FMA2(5)
        Ap += MR;
        Bp += NR;
    }
    if(mr == MR && nr == NR){
        ACC_ROW(0) ACC_ROW(1) ACC_ROW(2) ACC_ROW(3) ACC_ROW(4) ACC_ROW(5)
    } else {
        float tmp[MR][NR];
        SPILL_ROW(0) SPILL_ROW(1) SPILL_ROW(2) SPILL_ROW(3) SPILL_ROW(4) SPILL_ROW(5)
        for(int r=0; r<mr; r++)
            for(int c=0; c<nr; c++)
                C[(size_t)r*ldc + c] += tmp[r][c];
    }
}

#define DA(r) const float *a##r = A + (size_t)(i + ((r) < mr ? (r) : mr-1))*K;

#define F16(r) \
    a = _mm256_broadcast_ss(a##r + k); \
    c##r##0 = _mm256_fmadd_ps(a, b0, c##r##0); \
    c##r##1 = _mm256_fmadd_ps(a, b1, c##r##1);

#define ST16(r) \
    _mm256_storeu_ps(C + (size_t)(i+(r))*N + p,     c##r##0); \
    _mm256_storeu_ps(C + (size_t)(i+(r))*N + p + 8, c##r##1);

#define SP16(r) _mm256_storeu_ps(tmp[r], c##r##0); _mm256_storeu_ps(tmp[r]+8, c##r##1);

__attribute__((target("avx2,fma")))
static void gemm_n16_avx2(const float *A, const float *B, float *C, int M, int K, int N){
    int np = (N + 15) / 16;
    float *Bp = get_buf(&tl_B, &tl_Bcap, (size_t)K * np * 16);
    for(int q=0; q<np; q++){
        int p = q*16, nr = (N-p < 16) ? N-p : 16;
        float *dst = Bp + (size_t)q*K*16;
        for(int k=0; k<K; k++)
            for(int c=0; c<16; c++)
                dst[(size_t)k*16 + c] = (c<nr) ? B[(size_t)k*N + p + c] : 0.0f;
    }
    #ifdef _OPENMP
    // #pragma omp parallel for schedule(static) if(M >= 2048)
    #endif
    for(int i=0; i<M; i+=6){
        int mr = (M-i < 6) ? M-i : 6;
        DA(0) DA(1) DA(2) DA(3) DA(4) DA(5)
        for(int q=0; q<np; q++){
            int p = q*16, nr = (N-p < 16) ? N-p : 16;
            const float *bp = Bp + (size_t)q*K*16;
            __m256 c00 = _mm256_setzero_ps(), c01 = _mm256_setzero_ps();
            __m256 c10 = _mm256_setzero_ps(), c11 = _mm256_setzero_ps();
            __m256 c20 = _mm256_setzero_ps(), c21 = _mm256_setzero_ps();
            __m256 c30 = _mm256_setzero_ps(), c31 = _mm256_setzero_ps();
            __m256 c40 = _mm256_setzero_ps(), c41 = _mm256_setzero_ps();
            __m256 c50 = _mm256_setzero_ps(), c51 = _mm256_setzero_ps();
            for(int k=0; k<K; k++){
                __m256 b0 = _mm256_loadu_ps(bp);
                __m256 b1 = _mm256_loadu_ps(bp + 8);
                __m256 a;
                F16(0) F16(1) F16(2) F16(3) F16(4) F16(5)
                bp += 16;
            }
            if(mr == 6 && nr == 16){
                ST16(0) ST16(1) ST16(2) ST16(3) ST16(4) ST16(5)
            } else {
                float tmp[6][16];
                SP16(0) SP16(1) SP16(2) SP16(3) SP16(4) SP16(5)
                for(int r=0; r<mr; r++)
                    for(int c=0; c<nr; c++)
                        C[(size_t)(i+r)*N + p + c] = tmp[r][c];
            }
        }
    }
}

#define F8(r) c##r = _mm256_fmadd_ps(_mm256_broadcast_ss(a##r + k), b, c##r);
#define Z8(r) __m256 c##r = _mm256_setzero_ps();
#define SP8(r) _mm256_storeu_ps(tmp[r], c##r);

__attribute__((target("avx2,fma")))
static void gemm_n8_avx2(const float *A, const float *B, float *C, int M, int K, int N){
    float *Bp = get_buf(&tl_B, &tl_Bcap, (size_t)K * 8);
    for(int k=0; k<K; k++)
        for(int c=0; c<8; c++)
            Bp[(size_t)k*8 + c] = (c<N) ? B[(size_t)k*N + c] : 0.0f;
    #ifdef _OPENMP
    // #pragma omp parallel for schedule(static) if(M >= 2048)
    #endif
    for(int i=0; i<M; i+=12){
        int mr = (M-i < 12) ? M-i : 12;
        DA(0) DA(1) DA(2) DA(3) DA(4) DA(5) DA(6) DA(7) DA(8) DA(9) DA(10) DA(11)
        Z8(0) Z8(1) Z8(2) Z8(3) Z8(4) Z8(5) Z8(6) Z8(7) Z8(8) Z8(9) Z8(10) Z8(11)
        const float *bp = Bp;
        for(int k=0; k<K; k++){
            __m256 b = _mm256_loadu_ps(bp);
            F8(0) F8(1) F8(2) F8(3) F8(4) F8(5) F8(6) F8(7) F8(8) F8(9) F8(10) F8(11)
            bp += 8;
        }
        float tmp[12][8];
        SP8(0) SP8(1) SP8(2) SP8(3) SP8(4) SP8(5) SP8(6) SP8(7) SP8(8) SP8(9) SP8(10) SP8(11)
        for(int r=0; r<mr; r++)
            for(int c=0; c<N; c++)
                C[(size_t)(i+r)*N + c] = tmp[r][c];
    }
}

__attribute__((target("avx2,fma")))
static inline float hsum256(__m256 v){
    __m128 lo = _mm256_castps256_ps128(v);
    __m128 hi = _mm256_extractf128_ps(v, 1);
    __m128 s4 = _mm_add_ps(lo, hi);
    __m128 s2 = _mm_add_ps(s4, _mm_movehl_ps(s4, s4));
    __m128 s1 = _mm_add_ss(s2, _mm_shuffle_ps(s2, s2, 1));
    return _mm_cvtss_f32(s1);
}

__attribute__((target("avx2,fma")))
static inline float dot_avx2(const float *a, const float *b, int K){
    __m256 a0 = _mm256_setzero_ps(), a1 = _mm256_setzero_ps();
    __m256 a2 = _mm256_setzero_ps(), a3 = _mm256_setzero_ps();
    int k = 0;
    for(; k+32<=K; k+=32){
        a0 = _mm256_fmadd_ps(_mm256_loadu_ps(&a[k]),    _mm256_loadu_ps(&b[k]),    a0);
        a1 = _mm256_fmadd_ps(_mm256_loadu_ps(&a[k+8]),  _mm256_loadu_ps(&b[k+8]),  a1);
        a2 = _mm256_fmadd_ps(_mm256_loadu_ps(&a[k+16]), _mm256_loadu_ps(&b[k+16]), a2);
        a3 = _mm256_fmadd_ps(_mm256_loadu_ps(&a[k+24]), _mm256_loadu_ps(&b[k+24]), a3);
    }
    for(; k+8<=K; k+=8)
        a0 = _mm256_fmadd_ps(_mm256_loadu_ps(&a[k]), _mm256_loadu_ps(&b[k]), a0);
    float s = hsum256(_mm256_add_ps(_mm256_add_ps(a0, a1), _mm256_add_ps(a2, a3)));
    for(; k<K; k++)
        s += a[k] * b[k];
    return s;
}

#define G8(r) s##r = _mm256_fmadd_ps(_mm256_loadu_ps(p##r + k), bv, s##r);
#define T8(r) t##r += p##r[k] * bk;

__attribute__((target("avx2,fma")))
static void gemv_avx2(const float *A, const float *B, float *C, int M, int K){
    int nb = (M + 7) / 8;
    #ifdef _OPENMP
    // #pragma omp parallel for schedule(static) if(M >= 2048)
    #endif
    for(int bi=0; bi<nb; bi++){
        int i = bi*8;
        int rr = (M-i < 8) ? M-i : 8;
        if(rr == 8){
            const float *p0 = A + (size_t)i*K;
            const float *p1 = p0 + K; const float *p2 = p1 + K; const float *p3 = p2 + K;
            const float *p4 = p3 + K; const float *p5 = p4 + K; const float *p6 = p5 + K;
            const float *p7 = p6 + K;
            __m256 s0 = _mm256_setzero_ps(), s1 = _mm256_setzero_ps();
            __m256 s2 = _mm256_setzero_ps(), s3 = _mm256_setzero_ps();
            __m256 s4 = _mm256_setzero_ps(), s5 = _mm256_setzero_ps();
            __m256 s6 = _mm256_setzero_ps(), s7 = _mm256_setzero_ps();
            int k = 0;
            for(; k+8<=K; k+=8){
                __m256 bv = _mm256_loadu_ps(B + k);
                G8(0) G8(1) G8(2) G8(3) G8(4) G8(5) G8(6) G8(7)
            }
            float t0 = hsum256(s0), t1 = hsum256(s1), t2 = hsum256(s2), t3 = hsum256(s3);
            float t4 = hsum256(s4), t5 = hsum256(s5), t6 = hsum256(s6), t7 = hsum256(s7);
            for(; k<K; k++){
                float bk = B[k];
                T8(0) T8(1) T8(2) T8(3) T8(4) T8(5) T8(6) T8(7)
            }
            C[i] = t0; C[i+1] = t1; C[i+2] = t2; C[i+3] = t3;
            C[i+4] = t4; C[i+5] = t5; C[i+6] = t6; C[i+7] = t7;
        } else {
            for(int r=0; r<rr; r++)
                C[i+r] = dot_avx2(A + (size_t)(i+r)*K, B, K);
        }
    }
}
#endif

static int cpu_has_avx2(void){
#ifdef X86
    static int cached = -1;
    if(cached < 0){
        cached = __builtin_cpu_supports("avx2") && __builtin_cpu_supports("fma");
    }
    return cached;
#else
    return 0;
#endif
}

void student_gemm(const float *A, const float *B, float *C, int M, int K, int N){
    mk_fn mk = mk_scalar;
#ifdef X86
    int avx2 = cpu_has_avx2();
    if(avx2) mk = mk_avx2;
#endif

    if(N == 1){
#ifdef X86
        if(avx2){ gemv_avx2(A, B, C, M, K); return; }
#endif
        gemv_scalar(A, B, C, M, K);
        return;
    }

#ifdef X86
    if(avx2 && N <= 64){
        if(N <= 8) gemm_n8_avx2(A, B, C, M, K, N);
        else       gemm_n16_avx2(A, B, C, M, K, N);
        return;
    }
#endif

    for(int i=0; i<M; i++)
        memset(&C[(size_t)i*N], 0, (size_t)N*sizeof(float));

    for(int jc=0; jc<N; jc+=NC){
        int nc = (N-jc < NC) ? N-jc : NC;
        for(int pc=0; pc<K; pc+=KC){
            int kc = (K-pc < KC) ? K-pc : KC;

            float *Bpack = get_buf(&tl_B, &tl_Bcap, (size_t)kc * ((nc+NR-1)/NR*NR));
            pack_B(B, N, pc, kc, jc, nc, Bpack);

            #ifdef _OPENMP
            // #pragma omp parallel for schedule(dynamic)
            #endif
            for(int ic=0; ic<M; ic+=MC){
                int mc = (M-ic < MC) ? M-ic : MC;
                float *Apack = get_buf(&tl_A, &tl_Acap, (size_t)((mc+MR-1)/MR*MR) * kc);
                pack_A(A, K, ic, mc, pc, kc, Apack);

                for(int jr=0; jr<nc; jr+=NR){
                    int nr = (nc-jr < NR) ? nc-jr : NR;
                    for(int ir=0; ir<mc; ir+=MR){
                        int mr = (mc-ir < MR) ? mc-ir : MR;
                        mk(kc,
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
