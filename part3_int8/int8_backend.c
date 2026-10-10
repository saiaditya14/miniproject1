#include <stdint.h>
#include "student_gemm_int8.h"

typedef struct {
    int8_t *q;
    float *s;
} QuantizedTensor;

extern int GS;

void matmul(float *xout, QuantizedTensor *x, QuantizedTensor *w, int n, int d) {
    student_gemm_int8(w->q, w->s, x->q, x->s, xout, d, n, 1, GS);
}
