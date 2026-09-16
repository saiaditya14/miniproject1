#include <immintrin.h>
#include <stdio.h>

int main(){
    float a = 3.0f;
    float b[8] = {1,2,3,4,5,6,7,8};
    float out[8];

    __m256 va  = _mm256_set1_ps(a);      // broadcast one scalar into all 8 lanes
    __m256 vb  = _mm256_loadu_ps(b);     // load 8 CONTIGUOUS floats into one register
    __m256 acc = _mm256_setzero_ps();    // 8 zeros, this is your accumulator

    acc = _mm256_fmadd_ps(va, vb, acc);  // acc[i] = va[i]*vb[i] + acc[i], all 8 lanes, ONE instruction

    _mm256_storeu_ps(out, acc);          // write the 8 lanes back to normal memory

    for(int i=0;i<8;i++) printf("%.1f ", out[i]);
    printf("\n");
    return 0;
}