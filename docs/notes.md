# GEMM optimization notes

## Reference architecture (industry standard)

- BLIS: https://github.com/flame/blis — the block+pack+microkernel architecture
  our "best version" is modeled on (MC/KC/NC blocking, pack A/B into contiguous
  panels, small register-blocked microkernel, SIMD inside the microkernel).
- How-to-optimize-GEMM (FLAME group, same lineage as BLIS): 
  https://github.com/flame/how-to-optimize-gemm/wiki — the actual incremental
  naive -> blocked -> packed -> vectorized -> threaded walkthrough, step by step.
  This is basically the same progression as our V1..V7 plan below.
- Goto & van de Geijn, "Anatomy of High-Performance Matrix Multiplication" —
  the original paper behind the BLIS/GotoBLAS blocking strategy.

## Build/learning plan (each step = one working, benchmarked version)

- [x] V1 — naive `ijk`
- [x] V2 — reorder to `ikj` (fix stride-N access on B)
- [x] V3 — tiling on top of `ikj` (cache blocking)
- [x] V4 — packing (copy tile of A/B into contiguous scratch before compute).
      Known issue, deliberately deferred to V5: packing granularity == compute
      tile size right now, so A/B get re-packed for every leaf tile with no
      reuse. Real fix needs two block sizes (pack a big macro-block once,
      compute many small micro-tiles from it) — rolling that into V5 instead
      of restructuring twice.
- [x] V5 — register blocking / microkernel. Proved with a 2x2 isolated
      microkernel (`micro2x2` returns a struct of 4 products for one k,
      `micro2x2_tile` accumulates over all k) — verified via -O2 assembly
      that it fully inlines to pure register (xmm) accumulation, zero `call`
      instructions. Not yet generalized to slide across the whole matrix or
      merged with V4's packing (still open, see V4 note above) — deferred,
      isolated proof of concept was judged sufficient to move on.
- [x] V6 — SIMD (AVX2/FMA intrinsics on the microkernel from V5). Built as a
      2x8 microkernel (`micro1x8`, two rows x one 8-wide vector of columns),
      verified against `lmao` after fixing two real bugs found along the way:
      (1) redeclaring the accumulator inside the loop (shadowing, caught by
      `-Wshadow`) silently reset it every iteration instead of accumulating;
      (2) `acc1`'s update fed back `acc` instead of `acc1`, corrupting row 1
      with row 0's running total. Both caught by diffing against `lmao`'s
      output, not just reading the code.
- [x] V7 — OpenMP. Not hand-coded in `part1_cpu/loop.c` — concept + the real landmine
      both understood and discussed: `#pragma omp parallel for` on the tile
      loop is close to free in principle, BUT `ap`/`bp`/`cp` are currently
      global/shared, so parallelizing as-is would race; real version needs
      thread-private packing buffers (declared inside the parallelized loop
      body, or similar). Deferred writing this to the real `part1_cpu/student_gemm.c`
      rather than grinding it out in the toy file.
- [ ] V8 — tuning sweep: try a few MC/KC/NC/MR/NR values on the real shapes,
      keep the fastest

Each version is a small diff on the previous one, not a rewrite — and we keep
all of them, since the GFLOP/s progression V1->V7 is itself good material for
the "explain your optimization strategy" writeup the assignment wants.

## Idea: shape-specialized fast paths on top of the generic kernel

The assignment publishes the exact shapes graded on:

| name | M | K |
|---|---|---|
| attn-proj | 288 | 288 |
| ffn-up | 768 | 288 |
| ffn-down | 288 | 768 |
| classifier | 32000 | 288 |

All dims are multiples of 32 (stated in the assignment). Plan:

1. Keep a fully generic `student_gemm(A,B,C,M,K,N)` (the V1-V8 kernel above)
   that's correct for *any* M,K,N — this is the safety net / fallback.
2. Add a dispatch at the top of `student_gemm` that checks if `(M,K)` matches
   one of the 4 known shapes, and if so jumps to a specialized version for
   that exact shape:
   - tile sizes chosen to divide the known dimension exactly (no remainder /
     boundary-clamping code needed -> fewer branches, more unrolling)
   - loop bounds as compile-time constants where possible -> compiler can
     unroll/vectorize harder than it can for a generic runtime M/K/N
3. If `(M,K)` doesn't match any known shape, fall through to the generic
   kernel — never silently wrong, just slower on unseen shapes.
4. Benchmark specialized vs generic on the known shapes to confirm the
   specialization actually earns its complexity (only keep it if it measurably
   wins).

This is legitimate "know your benchmark" engineering (same category as how
production BLAS libraries dispatch per-shape), not harness-gaming — it only
uses information the assignment itself publishes.

## Status: real files written

- `part1_cpu/student_gemm.c` — the real float32 contract. V1-V7 techniques from
  `part1_cpu/loop.c` assembled together (ikj, macro-block packing reused across many
  microkernel calls, 4x8 register-blocked AVX2/FMA microkernel, OpenMP with
  per-thread packing buffers). Verified correct against a naive reference on
  all 4 real shapes x {N=1, N=32}, maxdiff ~1e-5. Verified via -O3 assembly:
  zero calls in the hot path. Added a dedicated `gemv()` path for N=1 (plain
  GEMM microkernel wastes 7/8 of its width when N=1) -- classifier N=1 went
  5.1 -> ~19 GFLOP/s (noisy, sub-ms problem size, but real average uplift).
  Still TODO: V8 tuning sweep; the OpenMP thread-safety fix is in the real
  file even though it was never hand-coded in part1_cpu/loop.c.
- `part1_cpu/student_gemm_int8.c` — real int8 contract. Second pass applied the same
  ikj lesson from the float kernel: original version accessed `Bq[k*N+j]`
  with j fixed inside a t-loop (stride-N, same bug as the very first naive
  GEMM before nulmao). Restructured to: per row, per group, broadcast
  `Aq[i][k]` and sweep j innermost into a small int32 accumulator array
  (stride-1 B access), scale-and-add into a float row buffer once per group,
  plus OpenMP over rows. No hand-written int8 SIMD intrinsics (signed x
  signed int8 dot product on AVX2 needs sign-extend-to-int16 + madd_epi16 to
  avoid overflow/saturation traps -- judged not worth the correctness risk
  given time; compiler auto-vectorizes the now-contiguous inner loop
  reasonably well on its own). Result: 10-50x over the correctness-first
  version on N=32 shapes (2 -> 19-118 GOP/s), maxdiff unchanged (<=0.0004).
  ASSUMED SCALE LAYOUT, still not confirmed against the real repo:
  `As[i*(K/GS)+g]` (per row, per K-group), `Bs[g*N+j]` (per K-group, per
  col) -- fix this indexing first thing once the GitHub Classroom repo
  lands and the real `student_gemm_int8.h` is available.
- Benchmarking gotcha found while measuring: these shapes are sub-millisecond
  at these sizes, so single-shot timing is noisy (saw the same N=32 shape
  swing 4.6 -> 20.2 -> 10.3 GFLOP/s across identical back-to-back runs).
  Real GFLOP/s numbers for the deliverable should average several runs, not
  trust one measurement.
- Benchmarked against real OpenBLAS 0.3.28 (median of 51 runs, 16 cores) --
  see BENCHMARKS.md. 24-62% of OpenBLAS on 3/4 shapes (expected/respectable
  for a from-scratch kernel), but genuinely BEAT OpenBLAS on the classifier
  shape (3.2-3.7x on N=1, 1.16-1.24x on N=32), reproduced across runs. Likely
  because gemv()/OpenMP-over-32000-rows is hand-tuned for exactly that access
  pattern in a way OpenBLAS's general dispatch isn't.

## Tactic: checking the professor's public course-site git history

With his explicit permission ("you can use anything on my git as well to any
advantage you want"), checked github.com/sureshpurini/fpga-monsoon26-web
(default branch `master`) commit history for anything touching
projects/mini1-gemm/. Found two commits from Sep 7 2026 that blanked out
reference numbers specifically so profiling/benchmarking would be a student
exercise (`c8b2393` "Part 1: blank reference perf numbers", `cba4809`
"remove verified-numbers section"). This gave an actual FLOOR to check our
work against, not just our own measurements in isolation -- worth doing
this kind of check again if progress ever feels ungrounded.

**Pulled reference floor numbers (naive reference implementation, N=1):**

Float32 GFLOP/s: attn-proj 0.81, ffn-up 1.66, ffn-down 1.61, classifier 1.54.
int8 GOP/s (GS=32): attn-proj 2.4, classifier 2.4.

**Our numbers vs that floor (float32, after fixes):**
attn-proj 2.12 (2.6x), ffn-up 4.30 (2.6x), ffn-down 4.72 (2.9x), classifier
22.99-32.00 (15-20x). All solidly beating the floor.

**int8 vs floor -- this caught a real regression:** attn-proj N=1 was
initially measured at 0.19 GOP/s, i.e. 12x WORSE than the 2.4 floor. Root
cause: `student_gemm_int8` had no dedicated N=1 path (unlike the float
kernel's `gemv()`) -- it ran the full JBLK-blocked machinery even for a
288x288x1 problem, and once a `gemv_int8()` path was added, OpenMP
thread-spawn overhead ALONE still dominated at that size (168K total ops
split across 16 threads is nothing per thread). Fixed with two changes:
1. added `gemv_int8()`, mirroring the float kernel's `gemv()`
2. added `if(M >= 2048)` to the `#pragma omp parallel for` so tiny M runs
   single-threaded instead of paying thread-spawn cost for no benefit
Result: attn-proj N=1 0.19 -> 1.81 GOP/s (floor 2.4, now close instead of
12x under), classifier N=1 4.96 -> 35.08 GOP/s (14.6x the floor). This is
exactly the kind of bug that's invisible without an external floor to check
against -- our own synthetic benchmark alone never flagged it as wrong,
just "not that fast," which didn't stand out until compared to a real
reference number.

## Brain-dump: specific things the known shapes let us exploit

Unsorted, unverified, to try/benchmark later — not yet implemented.

- **3 of 4 shapes share K=288.** attn-proj, ffn-up, and classifier are all
  K=288; only ffn-down is K=768. Worth having one shared, hyper-specialized
  "K288" inner kernel used by three of the four cases, rather than treating
  all four as unrelated.
- **288 and 768 are both very divisible.** 288 = 9*32 = 36*8; 768 = 24*32 =
  96*8. Both divide cleanly by 8 (SIMD width), 32 (stated dim guarantee), and
  a bunch of common tile sizes (32/64/96/128/192) with zero remainder. Means
  we can pick tile sizes for the *known* shapes with literally no boundary/
  clamping code at all — that generic `min()` clamping logic in the current
  kernel is pure overhead on these specific shapes.
- **classifier (M=32000, K=288) is the "very tall skinny" outlier.** B here
  is only `288*N` floats — for small N this is tiny (e.g. N=1 -> 1152 bytes,
  N=32 -> 36KB) and comfortably fits L1/L2 entirely. So the right strategy
  for this shape is probably: keep all of B resident on-chip once, then
  stream the 32000 rows of A through it — barely any of the MC/KC/NC block
  looping is needed since B never needs to be re-fetched from RAM. This is
  the CPU analogue of the lecture's row-wise/Gustavson "B lives in BRAM" win.
  Also the one shape where OpenMP parallelism (splitting 32000 rows across
  cores) obviously pays for its thread-launch overhead — the other 3 shapes
  are small enough that threading might not be worth it (need to benchmark).
- **attn-proj (288x288) may not need multi-level cache blocking at all.**
  Whole problem is ~330KB per matrix — plausibly fits in L2/L3 simultaneously.
  Might be fastest to just pack once and run the microkernel directly, no
  outer MC/KC/NC loop nest.
- **N=1 (memory-bound case) shouldn't use the same architecture as N>1 at
  all.** The whole point of packing B into NR-wide panels is to reuse a
  loaded B value across multiple output columns — with N=1 there's only one
  column, so there's nothing to reuse and the packing/microkernel machinery
  is pure overhead. N=1 wants its own much simpler kernel: per-row SIMD dot
  product against one resident B vector, parallelized over rows.
- **int8 group size GS=32 lines up with K's divisibility.** If we choose the
  int8 kernel's K-tile size as a multiple of 32, quantization groups never
  get split across a tile boundary — one clean group-accumulate-then-scale
  per K-tile, no partial-group bookkeeping.
- **Open question, needs the real repo to check:** does the benchmark/test
  harness call `student_gemm` on the *same* B matrix repeatedly (e.g. timing
  N iterations of the same shape for a stable GFLOP/s number)? If so,
  caching a packed copy of B across calls (keyed by pointer) would be a huge
  legitimate win — this is literally what real LM inference serving does
  (weights are packed once, reused for every token). Need to see
  `test_gemm.sh` to know if this is even observable/fair, or if it's closer
  to the "gaming" line we discussed — flag for later, don't build yet.

## Oct 5: real starter kit landed (mini1-gemm-starter.zip) -- what it changed

Contract confirmed against the real headers/reference:
- `student_gemm` signature identical. int8 scale layout we ASSUMED is exactly
  right: `As[i*(K/GS)+g]`, `Bs[g*N+j]` (the .h formula comment says Bs[j,g] but
  the stated layout and the reference code are g*N+j -- code is authoritative).
- Official tests: `gcc -O3 ... -lm` ONLY. No -march/-mavx2/-fopenmp, "one CPU
  core", single cold call timed. Everything we measured before with
  `-march=native -fopenmp` was NOT what gets scored.
- Tolerances: float 1e-2 absolute; int8 1e-4 PER-ELEMENT RELATIVE, and the
  reference is bit-exact against its golden.

Bugs the official harness caught that our own benchmarks had hidden:
1. int8 FAILED the real tester (4/8 cases). We multiplied `dot*As*Bs`; the
   reference does `(As*Bs)*dot`. Different float rounding order -> relative error
   blows up on near-zero outputs (we had seen maxrel=0.14 in our own bench and
   dismissed it as a near-zero artifact -- wrong, the official tester counts
   exactly that). Fix: match the reference's operation order. Now bit-exact.
2. Our AVX2/FMA + OpenMP were gated on compile flags the harness never passes,
   so the graded build silently ran the scalar fallback. Fix: function-level
   `__attribute__((target("avx2,fma")))` + runtime `__builtin_cpu_supports`
   dispatch (no build flags needed, scalar fallback if CPU lacks it).
3. attn-proj N=1 was BELOW the reference floor (0.64 vs 2.47) only on the first
   call. Steady state was 16us vs reference 78us; first call was 635us. Cause:
   `float Bpack[NC*KC]; float Apack[MC*KC];` as locals = ~640KB stack frame;
   Windows page-faults the whole frame on first call even when the N=1 path
   returns early. Fix: grow-only thread-local malloc'd buffers sized to need.
   Found by timing 5 consecutive calls instead of guessing (two earlier guesses
   -- cpuid cost, AVX warm-up -- were wrong and disproved by experiments).
4. N=1 float path: scalar gemv was latency-bound on a single accumulator;
   now 8-lane / 4x-vector accumulators. OpenMP in gemv gated on M>=2048.

Where we stand (details in BENCHMARKS.md): official float PASS 8/8, ~2-7x
reference depending on shape/N; int8 PASS bit-exact at N=1/8/32/64, ~1.2-6x
reference (N=8 is weakest). vs OpenBLAS (sgemv for N=1, sgemm for N=32): only
classifier N=32 is a clear win (1.18x); classifier N=1 ties; the rest are
38-69%. CORRECTION: an earlier note here claimed 2.5-3.4x wins at N=1 -- that
was vs sgemm-on-a-vector, an unfair baseline; retracted.

Still open: int8 has no AVX2 path (compiler auto-vectorizes at SSE2 baseline
under the official flags) -- the obvious next win is target("avx2") on the int8
inner loops; V8 tuning sweep; strategy writeup.
New in the starter kit worth reading before Part 2/3: slm_backend.c,
hardware-run.md, final-starter (kernel.cpp, hls_config.cfg, Makefile), and the
website says the graded submission involves a U50 bitstream (Moodle handout).

## Oct 5 (later): optimisation round -- what moved the numbers

Yardstick fix first: this OpenBLAS is an OpenMP build, so OPENBLAS_NUM_THREADS=1
is ignored (reported 16 threads) -- earlier "single-core" OpenBLAS numbers were
secretly 16-thread. Use OMP_NUM_THREADS=1. Also the unfair-baseline retraction:
N=1 must be compared against sgemv, not sgemm (BENCHMARKS.md has the details).

1. Float microkernel 4x8 -> 6x16 (12 accumulators, 6 broadcasts x 2 B vectors,
   MC=120/KC=256/NC=512, full tiles accumulate straight into C instead of via a
   tmp copy). Reason: 4 independent FMA chains cannot hide FMA latency (~4
   cycles x 2 ports needs >=8). Single-core vs OpenBLAS at N=32: 0.5-0.6x ->
   ~0.8-0.9x; official-harness N=32 roughly doubled. Passes 16/16 + 28/28.
2. int8 AVX2 path: sign-extend int8->int16, _mm256_madd_epi16 on interleaved
   B row pairs (two k at once per instruction), int32 accumulate, scale at the
   group boundary. Compiled with target("avx2") ONLY (no fma) on purpose: with
   FMA enabled gcc may contract the float mul+add and break the bit-exactness
   the 1e-4 relative tester needs. Added an 8-wide strip because N=8 was
   falling entirely into the scalar tail (4 -> 15-21 GOP/s). Bit-exact at
   N=1,3,7,8,24,33,40,64,100. 5-25x the reference depending on shape/N.
3. Not done / ideas left: AVX-VNNI (this CPU has it, grader unknown), tuning
   MC/KC/NC (V8), prefetch for N=1, shape-specialised dispatch, packing C
   writeback fusion. Float is now near OpenBLAS parity on one core, so further
   float gains will be small and hard to measure on this noisy machine.

## Oct 5 (latest): shape/N specialisation vs OpenBLAS

Question was whether overfitting to the known shapes could beat BLAS. Answer:
yes, modestly and only in the regime we specialise (N <= 64, these four
shapes, single thread). Changes in part1_cpu/student_gemm.c:
- N 9..64: `gemm_n16_avx2` -- pack only B (K x N, tiny), read A directly from 6
  row streams (no A packing), full K in one pass (no KC blocking, C written once,
  so no memset/accumulate). N 2..8: `gemm_n8_avx2`, 12 rows x 8 cols so the
  vector is not half wasted. N=1: 8-row gemv sharing each B load across 8 FMAs.
  N > 64 still uses the general packed 6x16 path.
- Median ratio vs single-thread OpenBLAS over 3 runs: N=8 1.2-1.8x, N=32
  1.13-1.22x, N=64 1.06-1.12x, N=1 0.97-1.27x (attn-proj N=1 is a tie).
- Verified: official float 16/16, 64/64 over 16 N values, 847 random (M,K,N)
  combos vs a naive double reference (C pre-filled with garbage to prove the
  new assign-not-accumulate paths overwrite everything), int8 still bit-exact.
  ASan/UBSan not available on this MinGW, so no sanitizer run.
Resume-safe wording: "shape-specialised AVX2 GEMM; ~1.1-1.8x single-thread
OpenBLAS sgemm/sgemv on four fixed shapes for N<=64" -- NOT "beats BLAS".
Untested: N>64, other shapes, other CPUs. Open ideas: tune MC/KC/NC for the
N>64 path, AVX-VNNI int8 (CPU has it, grader unknown).

## Oct 5: Part 2 seam verified end to end (CPU, TinyStories 15M)

Built llama2.c + stories15M.bin in a scratch dir and ran the course's own seam
(slm_backend.c: matmul -> student_gemm(w, x, xout, d, n, N=1)) with plain
`gcc -O3`. Same prompt/seed/-t 0:
- generated story BYTE-IDENTICAL to stock llama2.c, with both the reference
  GEMM and ours (correctness through the real model confirmed).
- tok/s over 5 runs, same machine: reference GEMM 59-88, ours 181-253
  (~2.5-3.5x). Stock run.c ~106 (single run).
Environment notes (not our code): GCC 14 turns implicit-function-declaration
(the renamed matmul) and incompatible-pointer-types (llama2.c's win.c shim on
Windows) into hard errors; I built with -Wno-error=... for both and win.c added
to the link. The course scripts will hit the first one on any GCC >= 14.
Still to do for Part 2: scripts/profile.sh + the Amdahl/"which shape dominates"
questions on his site; the U50 hardware run is PART 2 section 2.4/2.6 (not separate, not Part 3): bitstream built on RCE, host seam student_gemm_fpga.cpp, submitted via portal, tok/s comes back as 'your real hardware number for the Part 2 write-up'.
Part 3 = CPU-side int8: runq golden + an int8_backend.c seam we write ourselves (not in the zip) + precision sweep (int8 GS=64/32, int4) on 3 axes: tok/s, memory, story quality. No hardware in Part 3.

## Part 2 profile (course profile.sh method, 256 tokens greedy, 15M model)

Measured with the course's mkprof.py wrapper plus one added elapsed-time line so
f is measured, not estimated. Five runs each, this laptop, plain gcc -O3.

Question 1 -- matmul fraction f (stock matmul): 91.8-95.7% of runtime (mean ~94%).
Amdahl cap for accelerating only GEMMs: 1/(1-f) = ~12x (f=.918) to ~23x
(f=.957), ~17x at the mean. Our kernel got ~2.5-3.5x (tok/s 73-104 -> 214-301),
far below the cap. With OUR kernel plugged in, f drops to 82-92% (mean ~86%):
the non-matmul remainder (norms, softmax, sampling, startup) is now 8-18% of
the run, so a further infinitely-fast GEMM would cap at ~7x over our current.
(elapsed includes model/tokenizer load and printing, so f is slightly low.)

Question 2 -- which shape dominates (share of matmul time, stock):
  288->32000 classifier 64.9% | 288->768 (w1,w3) 15.3% | 288->288 (attn) 10.3%
  | 768->288 (w2) 9.5%.   (calls: 222 / 2664 / 5328 / 1332)
So the classifier dominates, NOT the FFN (FFN total ~25%): "profile, don't
assume" -- the 32000-token vocab makes the classifier the FLOP monster even
though it is called once per token. With our kernel: classifier 59.9%, w1/w3
29.2%, attn 6.5%, w2 4.4% (the 288->768 shape is now relatively the slowest
per FLOP -- worth a look). Hardware implication: if only the top of the profile
is offloaded, the classifier alone is ~2/3 of matmul time.
His removed reference numbers said 71.6 tok/s and f=84% on his machine; ours
reads 73-104 tok/s and f~94% -- same story, different machine/compiler.

## Part 2, everything before the FPGA (done on CPU, Oct 5-6)

Files (repo): part2_batching/batch_bench.c (batched forward pass, #includes llama2.c's
run.c unchanged), part2_batching/slm_backend_select.c (offload only chosen shapes).
Build (Windows/GCC14): gcc -O3 -w -Wno-error=incompatible-pointer-types
-Wno-error=implicit-function-declaration -I<llama2.c> -I<starter/src>
part2_batching/batch_bench.c <llama2.c>/win.c part1_cpu/student_gemm.c -lm ; run from the llama2.c
dir (tokenizer.bin is read from CWD). Usage: batch_bench model.bin B steps [check|show].

1. build_slm seam: story BYTE-IDENTICAL to stock with ours (done earlier).
2. Profile + Amdahl (see section above): f ~94% stock, classifier 65% of matmul.
3. Batched forward (B independent streams, each matmul = student_gemm with N=B,
   activations gathered into K x B matrices; RoPE/attention/norms stay
   per-stream). Verified vs stock forward(): worst logit diff 4.5e-5, 0 argmax
   mismatches in 704 comparisons (B = 1,3,8,32; 16 steps).
4. tok/s vs N (aggregate tokens/s, 256 steps, median of 5 interleaved runs,
   cheap sampler so top-p sorting does not pollute it):
     N=1 296 | N=2 263 | N=4 447 | N=8 729 | N=16 782 | N=32 510 | N=64 681
   (run-to-run spread is large on this laptop: e.g. N=8 427-837.)
   Time split at N=1: gemm ~89%; at N=16-64: gemm 41-49%, sampling ~15%,
   other (per-stream attention, rope, norms, gather/scatter) 36-44%.
   Reading: single-stream is memory-bound (N=2 is even slightly BELOW N=1:
   the N<=8 kernel pads to 8 lanes); batching lifts it ~2.5x to a ceiling of
   ~700-800 tok/s, after which the non-GEMM part dominates (Amdahl again).
   UNVERIFIED hypotheses for the N=32 dip: per-stream KV caches (3.5 MB each,
   113 MB at N=32) falling out of cache during attention. Not measured.
5. Per-shape offload step at N=1 (tok/s median of 7, stock naive -> ours):
     none 118 | classifier only 166 | +w1/w3 172 | +w2 248 | +attn (all) 288
   i.e. the classifier alone gives the biggest single step, consistent with it
   being 65% of matmul time. This is the offload priority list for the U50.
Not done / optional: prefill variant (N = prompt length), a plotted figure.
Remaining Part 2: the FPGA side only (HLS kernel src/kernel.cpp top 'gemm',
host seam student_gemm_fpga.cpp, hw bitstream on RCE ~1h, portal submission).
final-starter files (kernel.cpp, kernel_tb.cpp, hls_config.cfg, Makefile,
slurm.sh) not yet read.

## Oct 6: final-starter read (the graded FPGA deliverable)

mini1-gemm-final-starter.zip = "a GEMM kernel on the U50". Write ONE HLS kernel,
src/kernel.cpp, top `extern "C" void gemm(A,B,C,M,K,N)` (m_axi A/B/C, bundles
gmem0/1/2, M/K/N s_axilite, row-major float32). One bitstream must handle all
shapes: (M,K) in {288x288, 768x288, 288x768, 32000x288} at N=1 and 32, plus
square 768^3 (so K<=768, M<=32000, N<=768, any N>=1).
Workflow (README): make csim (secs) -> make cosim (mins; depth= in kernel.cpp
must equal COSIM_*_ELEMS in kernel_tb.cpp) -> ./slurm.sh hw (~1h, account
cs2401, build dir build/hw/gemm.xclbin) -> HW_TOKEN=<token> make submit (from
login node) -> results at http://10.4.25.39:18080/?token=... ; leaderboard at
/leaderboard (campus network). Clock target 3.33ns (300 MHz). Never build on
the login node (sinteractive -c 8 -A cs2401 for quick steps).
Scoring: 9 cases (all must be within 1e-2), three scores in ms of kernel time:
square (leaderboard ranks by this), N=1 sum of 4 shapes, N=32 sum of 4 shapes;
then a 256-token TinyStories run with every matmul on the kernel -> tok/s.
Placeholder = 2 tok/s (II=8), "simple tiled" ~7 tok/s, "well-optimized much
higher". Limits: 5 min for the 9 cases, 3 min for the story. Submissions run one
at a time (~1 min each); resubmitting while queued replaces the earlier one.
Needs from the instructor: a personal HW_TOKEN (do we have it?).
What I can/cannot do from this Windows box: write kernel.cpp and check functional
correctness by compiling kernel_tb.cpp with plain g++ (HLS pragmas are ignored);
CANNOT check II/timing/resources/cosim/hw -- those need RCE (Vitis 2025.1).
Hints from the README: N=1 is bandwidth-bound (wide ports, long bursts, many
MACs in flight); N>1: buffer tiles on-chip; split the accumulator into several
partial sums to reach II=1 (same L3/L4 ideas as the lecture handout).


## Oct 6 night: FPGA bring-up log (v3 -> v5)
- csim on RCE PASS (all 9). v2/v3 csynth: all loops II=1, A/B read as 512-bit bursts (max_widen_bitwidth=512 plus the
  aligned (base16+t)*16+q addressing trick works). The GEMV A-tile load was II=2 until a DEPENDENCE inter-false
  pragma was added. fadd core = 7-cycle DSP; fmul = 4-cycle.
- Timing diagnosis: both compute loops had a single `fadd`(2.342 ns)->store critical path (est. 2.729 ns) even
  with DEPENDENCE distance=L, because fully partitioned accumulators are scalar registers (distance-1 loop-carried
  for the tool). Fix attempted = ACC_STYLE=2: accumulators as LUTRAM partitioned only over (row,col) with
  the rotating index so the pragma applies and the 7-cycle adder can be used. Result pending (v4 csynth).
- Cosim: first attempt on the login node crashed xelab with LLVM out-of-memory (never build/simulate on `rce`,
  use `./slurm.sh <target>`: 32 GB). chmod +x on slurm.sh/env.sh/scripts after uploading from Windows.
  v3 cosim PASS via Slurm in ~10 min; measured cycle counts and the fitted overhead model are in CLAUDE.md.
- Process lesson: the user wants stepwise validation (see CLAUDE.md working rules).

## Oct 7: first measured hardware result (v3, submission id 58)
- v3 on the U50 (kernel clock auto-scaled 300 -> 279.1 MHz after timing failed by 0.249 ns; worst path was the pipeline-enable CE fanout, 94% routing): all 9 cases PASS.
- square 32.70 ms (27.7 GFLOP/s), N=1 sum 8.80 ms, N=32 sum 24.03 ms, TinyStories 57.2 tok/s.
- N=1 (32000x288) 8.29 ms = 4.45 GB/s of A, about 24% of the 17.9 GB/s single port at 279 MHz; about 578 cycles per 8-row tile vs 144 ideal. Cosim-based fit (about 319 cycles/tile) under-predicted: real HBM latency is higher than the cosim memory model.
- Model retraction: pre-run estimates (square 20-25 ms, N=1 6.5-7 ms, N=32 14-18 ms) were optimistic by about 1.3-1.7x. Use the measured numbers as the baseline.
- Leaderboard context at the time: rank 1 square 1.166 ms (777 GFLOP/s), top 3 square 5.05-6.45 ms, N=1 3.2-3.65 ms, N=32 3.46-4.38 ms, tok/s 104-119.
- Variants prepared (rce_upload/): v4, v4s0 (ACC_STYLE=0), v5a/v5b (dataflow, RT=4/8), *_p probes with extra cosim cases, v5b_frp/v12_frp/v6_frp (style=frp on the two hot compute loops). ACC_STYLE=2 is dead (LUTRAM read chained in front of fadd: 3.041 ns and 2x DSP).
