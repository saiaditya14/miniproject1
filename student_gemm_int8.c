#include <stdint.h>
#include <string.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define JBLK 512

void student_gemm_int8(const int8_t *Aq, const float *As,
                        const int8_t *Bq, const float *Bs,
                        float *C, int M, int K, int N, int GS){
    int NG = K / GS;

    #ifdef _OPENMP
    #pragma omp parallel for schedule(dynamic)
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
                    crow[jj] += (float)acc[jj] * ascale * Bsrow[jj];
            }

            memcpy(&C[(size_t)i*N+j0], crow, (size_t)jb*sizeof(float));
        }
    }
}
