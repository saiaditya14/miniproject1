#include "student_gemm.h"

#ifndef MASK
#define MASK 15
#endif

void matmul(float *xout, float *x, float *w, int n, int d) {
    int bit;
    if(d == 32000)      bit = 1;
    else if(d == 768)   bit = 2;
    else if(n == 768)   bit = 4;
    else                bit = 8;

    if(MASK & bit){
        student_gemm(w, x, xout, d, n, 1);
    } else {
        for(int i=0; i<d; i++){
            float val = 0.0f;
            for(int j=0; j<n; j++) val += w[i*n + j] * x[j];
            xout[i] = val;
        }
    }
}
