# Benchmark results — student_gemm / student_gemm_int8

Machine: i7-1260P laptop (AVX2 + AVX-VNNI, no AVX-512), MSYS2 gcc 14.2. Timings
drift ~2x between identical runs (short problems, hybrid P/E cores, background
load), so compare ratios inside one run, not absolute numbers across runs.

## 1. Official harness (course scripts, unmodified: plain `gcc -O3 -lm`)

No -march, no -mavx2, no -fopenmp, single core, one cold call timed. AVX2 is
selected at runtime inside the source. Reference = the shipped naive
`student_gemm_ref.c` / `student_gemm_int8_ref.c` through the same tester.

Float, best of 7 runs, GFLOP/s (tolerance 1e-2; official cases 16/16 PASS;
also 64/64 PASS over 16 values of N from 2 to 100, and 847 random
(M,K,N) combinations incl. odd M/K/N checked against a naive double reference,
~1M outputs, worst abs err 7e-5):

| Shape | N=1 | N=8 | N=32 | N=64 | Reference N=1 / N=32 |
|---|---|---|---|---|---|
| attn-proj | 11.1 | 70.6 | 59.0 | 96.4 | 2.47 / 4.38 |
| ffn-up | 20.7 | 63.8 | 98.1 | 110.8 | 3.18 / 3.69 |
| ffn-down | 22.9 | 82.5 | 88.1 | 97.2 | 1.63 / 3.30 |
| classifier | 10.6 | 49.9 | 81.8 | 96.3 | 4.17 / 3.78 |

Throughput climbs with N, as the assignment says a good implementation does;
the reference stays flat at ~2-4 GFLOP/s.

Int8, best of 5 runs, GOP/s (per-element relative tolerance 1e-4; bit-exact,
maxrel 0.0, at N=1,3,7,8,24,33,40,64,100):

| Shape | N=1 | N=8 | N=32 | N=64 | Reference N=1 / N=32 |
|---|---|---|---|---|---|
| attn-proj | 9.8 | 21.3 | 38.9 | 43.0 | 1.82 / 2.44 |
| ffn-up | 24.6 | 26.6 | 44.9 | 47.4 | 2.15 / 1.59 |
| ffn-down | 22.0 | 31.1 | 47.4 | 48.1 | 2.40 / 2.52 |
| classifier | 19.3 | 29.6 | 47.9 | 44.8 | 1.86 / 2.77 |

## 2. Against OpenBLAS 0.3.28, single thread vs single thread

`bench_openblas.c`, median of 51 calls per measurement, built like the official
harness (plain `gcc -O3`, no OpenMP, no -march), `OMP_NUM_THREADS=1` (this
OpenBLAS is an OpenMP build and ignores OPENBLAS_NUM_THREADS). OpenBLAS runs its
Haswell AVX2 kernels, the same ISA as ours. N=1 vs `cblas_sgemv`, N>1 vs
`cblas_sgemm`. Ratio = ours / OpenBLAS, median over 3 runs (min - max in
brackets):

| Shape | N=1 | N=8 | N=32 | N=64 |
|---|---|---|---|---|
| attn-proj | 0.97 (0.76-1.00) | 1.81 (1.81-1.99) | 1.22 (1.18-1.39) | 1.11 (1.06-1.59) |
| ffn-up | 1.27 (1.13-1.30) | 1.76 (1.50-1.85) | 1.20 (1.18-1.24) | 1.12 (1.07-1.22) |
| ffn-down | 1.24 (1.18-1.42) | 1.76 (1.53-1.91) | 1.18 (1.02-1.27) | 1.10 (1.00-1.22) |
| classifier | 1.11 (1.06-1.16) | 1.22 (1.07-1.40) | 1.13 (1.12-1.20) | 1.06 (1.03-1.19) |

What this does and does not show:
- It is a shape-specialised kernel against a general-purpose library call, on
  four fixed shapes and N <= 64. The gains come from things a generic library
  does not assume: no packing of A when N is small (A is a weight matrix read
  once), no K-blocking (K is only 288/768 so a row slab fits in L1), a 12x8
  tile when N <= 8 (a 16-wide strip would waste half its lanes), and 8 rows per
  pass in the N=1 gemv so one B load feeds 8 FMAs.
- Single thread only. Not measured: N > 64 (that goes through the general
  packed 6x16 path, which was ~0.8-0.9x of OpenBLAS in an earlier round),
  other shapes, other machines (the grader's CPU may differ; runtime dispatch
  falls back to scalar code if AVX2/FMA are missing).
- 14 of 16 cells are >= 1.0x by median, attn-proj N=1 is a tie. The noise
  floor on this laptop is large, so treat 1.0-1.1x as "about equal".

Retractions, so they are not repeated: (a) an early version claimed 2.5-3.7x
wins at N=1 -- that compared against `sgemm` on a vector, an unfair baseline.
(b) An early "1.18x on classifier N=32" was a 16-thread run where our
OpenMP-over-rows scaled better than OpenBLAS; it was not a kernel-quality win.

Reproduce: `gcc -O3 bench_openblas.c student_gemm.c -lopenblas -lm`, run with
`OMP_NUM_THREADS=1` (needs OpenBLAS; `pacman -S
mingw-w64-ucrt-x86_64-openblas` on MSYS2).
