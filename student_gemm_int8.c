#include <stdint.h>
#include <string.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#if defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#define X86 1
#endif

#define JBLK 512

static void gemv_int8_scalar(const int8_t *Aq, const float *As,
                              const int8_t *Bq, const float *Bs,
                              float *C, int M, int K, int GS){
    int NG = K / GS;
    #ifdef _OPENMP
    // #pragma omp parallel for schedule(static) if(M >= 2048)
    #endif
    for(int i=0; i<M; i++){
        float acc = 0.0f;
        for(int g=0; g<NG; g++){
            int32_t dot = 0;
            int k0 = g*GS;
            const int8_t *Arow = &Aq[(size_t)i*K+k0];
            const int8_t *Brow = &Bq[k0];
            for(int t=0; t<GS; t++)
                dot += (int32_t)Arow[t] * (int32_t)Brow[t];
            acc += As[(size_t)i*NG+g] * Bs[g] * (float)dot;
        }
        C[i] = acc;
    }
}

static void gemm_int8_scalar(const int8_t *Aq, const float *As,
                              const int8_t *Bq, const float *Bs,
                              float *C, int M, int K, int N, int GS){
    int NG = K / GS;

    #ifdef _OPENMP
    // #pragma omp parallel for schedule(dynamic)
    #endif
    for(int i=0; i<M; i++){
        for(int j0=0; j0<N; j0+=JBLK){
            int jb = (N-j0 < JBLK) ? N-j0 : JBLK;

            float crow[JBLK];
            memset(crow, 0, (size_t)jb*sizeof(float));

            for(int g=0; g<NG; g++){
                int32_t acc[JBLK];
                memset(acc, 0, (size_t)jb*sizeof(int32_t));

                int k0 = g*GS;
                for(int t=0; t<GS; t++){
                    int k = k0+t;
                    int32_t a = Aq[(size_t)i*K+k];
                    const int8_t *Brow = &Bq[(size_t)k*N+j0];
                    for(int jj=0; jj<jb; jj++)
                        acc[jj] += a * (int32_t)Brow[jj];
                }

                float ascale = As[(size_t)i*NG+g];
                const float *Bsrow = &Bs[(size_t)g*N+j0];
                for(int jj=0; jj<jb; jj++)
                    crow[jj] += ascale * Bsrow[jj] * (float)acc[jj];
            }

            memcpy(&C[(size_t)i*N+j0], crow, (size_t)jb*sizeof(float));
        }
    }
}

#ifdef X86
__attribute__((target("avx2")))
static void gemv_int8_avx2(const int8_t *Aq, const float *As,
                            const int8_t *Bq, const float *Bs,
                            float *C, int M, int K, int GS){
    int NG = K / GS;
    #ifdef _OPENMP
    // #pragma omp parallel for schedule(static) if(M >= 2048)
    #endif
    for(int i=0; i<M; i++){
        float acc = 0.0f;
        for(int g=0; g<NG; g++){
            const int8_t *Ar = &Aq[(size_t)i*K + (size_t)g*GS];
            const int8_t *Br = &Bq[(size_t)g*GS];
            __m256i vs = _mm256_setzero_si256();
            int t = 0;
            for(; t+16<=GS; t+=16){
                __m256i a = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i *)(Ar+t)));
                __m256i b = _mm256_cvtepi8_epi16(_mm_loadu_si128((const __m128i *)(Br+t)));
                vs = _mm256_add_epi32(vs, _mm256_madd_epi16(a, b));
            }
            __m128i s = _mm_add_epi32(_mm256_castsi256_si128(vs), _mm256_extracti128_si256(vs, 1));
            s = _mm_add_epi32(s, _mm_shuffle_epi32(s, 0x4E));
            s = _mm_add_epi32(s, _mm_shuffle_epi32(s, 0xB1));
            int32_t dot = _mm_cvtsi128_si32(s);
            for(; t<GS; t++)
                dot += (int32_t)Ar[t] * (int32_t)Br[t];
            acc += As[(size_t)i*NG+g] * Bs[g] * (float)dot;
        }
        C[i] = acc;
    }
}

__attribute__((target("avx2")))
static void gemm_int8_avx2(const int8_t *Aq, const float *As,
                            const int8_t *Bq, const float *Bs,
                            float *C, int M, int K, int N, int GS){
    int NG = K / GS;
    int N16 = N & ~15;

    #ifdef _OPENMP
    // #pragma omp parallel for schedule(dynamic)
    #endif
    for(int i=0; i<M; i++){
        float *Crow = &C[(size_t)i*N];

        for(int j=0; j<N16; j+=16){
            __m256 f0 = _mm256_setzero_ps(), f1 = _mm256_setzero_ps();
            for(int g=0; g<NG; g++){
                const int8_t *Arow = &Aq[(size_t)i*K + (size_t)g*GS];
                const int8_t *Brow = &Bq[(size_t)g*GS*N + j];
                __m256i d0 = _mm256_setzero_si256(), d1 = _mm256_setzero_si256();
                int t = 0;
                for(; t+2<=GS; t+=2){
                    __m128i r0 = _mm_loadu_si128((const __m128i *)(Brow + (size_t)t*N));
                    __m128i r1 = _mm_loadu_si128((const __m128i *)(Brow + (size_t)(t+1)*N));
                    __m256i e0 = _mm256_cvtepi8_epi16(_mm_unpacklo_epi8(r0, r1));
                    __m256i e1 = _mm256_cvtepi8_epi16(_mm_unpackhi_epi8(r0, r1));
                    int32_t pr = (int32_t)((uint32_t)(uint16_t)(int16_t)Arow[t] |
                                           ((uint32_t)(uint16_t)(int16_t)Arow[t+1] << 16));
                    __m256i ap = _mm256_set1_epi32(pr);
                    d0 = _mm256_add_epi32(d0, _mm256_madd_epi16(ap, e0));
                    d1 = _mm256_add_epi32(d1, _mm256_madd_epi16(ap, e1));
                }
                if(t < GS){
                    __m128i r0 = _mm_loadu_si128((const __m128i *)(Brow + (size_t)t*N));
                    __m128i z = _mm_setzero_si128();
                    __m256i e0 = _mm256_cvtepi8_epi16(_mm_unpacklo_epi8(r0, z));
                    __m256i e1 = _mm256_cvtepi8_epi16(_mm_unpackhi_epi8(r0, z));
                    __m256i ap = _mm256_set1_epi32((int32_t)(uint16_t)(int16_t)Arow[t]);
                    d0 = _mm256_add_epi32(d0, _mm256_madd_epi16(ap, e0));
                    d1 = _mm256_add_epi32(d1, _mm256_madd_epi16(ap, e1));
                }
                __m256 sa = _mm256_set1_ps(As[(size_t)i*NG+g]);
                const float *bs = &Bs[(size_t)g*N + j];
                __m256 s0 = _mm256_mul_ps(sa, _mm256_loadu_ps(bs));
                __m256 s1 = _mm256_mul_ps(sa, _mm256_loadu_ps(bs+8));
                f0 = _mm256_add_ps(f0, _mm256_mul_ps(s0, _mm256_cvtepi32_ps(d0)));
                f1 = _mm256_add_ps(f1, _mm256_mul_ps(s1, _mm256_cvtepi32_ps(d1)));
            }
            _mm256_storeu_ps(Crow + j,     f0);
            _mm256_storeu_ps(Crow + j + 8, f1);
        }

        int N8 = N16;
        if(N - N16 >= 8){
            int j = N16;
            __m256 f0 = _mm256_setzero_ps();
            for(int g=0; g<NG; g++){
                const int8_t *Arow = &Aq[(size_t)i*K + (size_t)g*GS];
                const int8_t *Brow = &Bq[(size_t)g*GS*N + j];
                __m256i d0 = _mm256_setzero_si256();
                int t = 0;
                for(; t+2<=GS; t+=2){
                    __m128i r0 = _mm_loadl_epi64((const __m128i *)(Brow + (size_t)t*N));
                    __m128i r1 = _mm_loadl_epi64((const __m128i *)(Brow + (size_t)(t+1)*N));
                    __m256i e0 = _mm256_cvtepi8_epi16(_mm_unpacklo_epi8(r0, r1));
                    int32_t pr = (int32_t)((uint32_t)(uint16_t)(int16_t)Arow[t] |
                                           ((uint32_t)(uint16_t)(int16_t)Arow[t+1] << 16));
                    d0 = _mm256_add_epi32(d0, _mm256_madd_epi16(_mm256_set1_epi32(pr), e0));
                }
                if(t < GS){
                    __m128i r0 = _mm_loadl_epi64((const __m128i *)(Brow + (size_t)t*N));
                    __m256i e0 = _mm256_cvtepi8_epi16(_mm_unpacklo_epi8(r0, _mm_setzero_si128()));
                    d0 = _mm256_add_epi32(d0, _mm256_madd_epi16(
                             _mm256_set1_epi32((int32_t)(uint16_t)(int16_t)Arow[t]), e0));
                }
                __m256 sa = _mm256_set1_ps(As[(size_t)i*NG+g]);
                __m256 s0 = _mm256_mul_ps(sa, _mm256_loadu_ps(&Bs[(size_t)g*N + j]));
                f0 = _mm256_add_ps(f0, _mm256_mul_ps(s0, _mm256_cvtepi32_ps(d0)));
            }
            _mm256_storeu_ps(Crow + j, f0);
            N8 = N16 + 8;
        }

        for(int j=N8; j<N; j++){
            float acc = 0.0f;
            for(int g=0; g<NG; g++){
                int32_t dot = 0;
                int k0 = g*GS;
                for(int t=0; t<GS; t++)
                    dot += (int32_t)Aq[(size_t)i*K+k0+t] * (int32_t)Bq[(size_t)(k0+t)*N+j];
                acc += As[(size_t)i*NG+g] * Bs[(size_t)g*N+j] * (float)dot;
            }
            Crow[j] = acc;
        }
    }
}
#endif

static int cpu_has_avx2(void){
#ifdef X86
    static int cached = -1;
    if(cached < 0)
        cached = __builtin_cpu_supports("avx2");
    return cached;
#else
    return 0;
#endif
}

void student_gemm_int8(const int8_t *Aq, const float *As,
                        const int8_t *Bq, const float *Bs,
                        float *C, int M, int K, int N, int GS){
#ifdef X86
    if(cpu_has_avx2()){
        if(N == 1) gemv_int8_avx2(Aq, As, Bq, Bs, C, M, K, GS);
        else       gemm_int8_avx2(Aq, As, Bq, Bs, C, M, K, N, GS);
        return;
    }
#endif
    if(N == 1) gemv_int8_scalar(Aq, As, Bq, Bs, C, M, K, GS);
    else       gemm_int8_scalar(Aq, As, Bq, Bs, C, M, K, N, GS);
}
