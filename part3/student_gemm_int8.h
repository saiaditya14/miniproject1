/* student_gemm_int8.h — the int8 contract (Part 1, and the Part 3 challenge).
 *
 * The int8 analogue of student_gemm. Group quantization (Q8_0, as in llama2.c's
 * runq.c): operands are int8 values plus one fp32 scale per group of GS elements;
 * the result is dequantized fp32.
 *
 *   C[i,j] = Σ_g ( As[i,g] · Bs[j,g] · Σ_{k∈group g} (int32) Aq[i,k]·Bq[k,j] )
 *
 * Aq : M×K int8      As : M×(K/GS) fp32   — left operand (weights) values + scales
 * Bq : K×N int8      Bs : (K/GS)×N fp32   — right operand (activations) values + scales
 * C  : M×N fp32
 *
 * Accumulate the inner products in int32 (that is where the FPGA wins — packed int8
 * MACs on the DSPs); apply the two scales at group boundaries. You implement this;
 * validate against runq.c's matmul (the golden), then plug it in via int8_backend.c.
 */
#ifndef STUDENT_GEMM_INT8_H_
#define STUDENT_GEMM_INT8_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void student_gemm_int8(const int8_t *Aq, const float *As,
                       const int8_t *Bq, const float *Bs,
                       float *C, int M, int K, int N, int GS);

#ifdef __cplusplus
}
#endif

#endif /* STUDENT_GEMM_INT8_H_ */
