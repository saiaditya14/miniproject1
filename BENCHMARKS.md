# Benchmark results — student_gemm (float32)

Methodology: median of 51 timed runs per shape, compared against OpenBLAS
0.3.28 (`cblas_sgemm`), on a 16-core machine, both using all cores.
Correctness verified to ~1e-5 max absolute error against OpenBLAS's output
on every shape. Single-shot timing was noisy on these problem sizes during
development (same shape swung several GFLOP/s across back-to-back runs) --
median-of-51 is the fix.

| Shape | N | Ours (GFLOP/s) | OpenBLAS (GFLOP/s) | Ratio |
|---|---|---|---|---|
| attn-proj (M=288, K=288) | 1 | 2.12 | 8.87 | 0.24x |
| attn-proj | 32 | 22.91 | 45.53 | 0.50x |
| ffn-up (M=768, K=288) | 1 | 4.30 | 7.00 | 0.61x |
| ffn-up | 32 | 51.70 | 112.98 | 0.46x |
| ffn-down (M=288, K=768) | 1 | 4.72 | 9.14 | 0.52x |
| ffn-down | 32 | 44.13 | 84.46 | 0.52x |
| classifier (M=32000, K=288) | 1 | 22.99 - 32.00 | 7.18 - 8.58 | 3.2x - 3.7x |
| classifier | 32 | 161.67 - 249.01 | 130.50 - 215.51 | 1.16x - 1.24x |

Classifier (the tall-skinny M=32000 shape) beats OpenBLAS, reproduced across
two independent runs. Likely explanation: `gemv()` (the dedicated N=1 path)
and OpenMP-over-32000-rows are hand-tuned for exactly this access pattern,
where OpenBLAS's general-purpose dispatch doesn't specialize as aggressively.
Not yet root-caused further than that -- worth digging into if there's time.

On the other three shapes, 24-62% of OpenBLAS's throughput -- expected and
reasonable for a from-scratch kernel against a professionally auto-tuned
library (see notes.md's discussion of why matching/beating vendor BLAS
generally isn't realistic).

int8 kernel: no OpenBLAS comparison yet (would need a reference int8 GEMM,
not just cblas_sgemm). Measured 10-50x speedup over the correctness-first
version on N=32 shapes after fixing the stride-1 access issue -- see
notes.md for detail.

Reproduce: `gcc -O3 -march=native -fopenmp student_gemm.c bench_rigorous.c
-lopenblas -lm` (bench_rigorous.c currently lives in the session's temp
scratchpad, not this repo -- recreate from this file's methodology if needed).
