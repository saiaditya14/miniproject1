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
- [x] V7 — OpenMP. Not hand-coded in `loop.c` — concept + the real landmine
      both understood and discussed: `#pragma omp parallel for` on the tile
      loop is close to free in principle, BUT `ap`/`bp`/`cp` are currently
      global/shared, so parallelizing as-is would race; real version needs
      thread-private packing buffers (declared inside the parallelized loop
      body, or similar). Deferred writing this to the real `student_gemm.c`
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

- `student_gemm.c` — the real float32 contract. V1-V7 techniques from
  `loop.c` assembled together (ikj, macro-block packing reused across many
  microkernel calls, 4x8 register-blocked AVX2/FMA microkernel, OpenMP with
  per-thread packing buffers). Verified correct against a naive reference on
  all 4 real shapes x {N=1, N=32}, maxdiff ~1e-5. Verified via -O3 assembly:
  zero calls in the hot path. Added a dedicated `gemv()` path for N=1 (plain
  GEMM microkernel wastes 7/8 of its width when N=1) -- classifier N=1 went
  5.1 -> ~19 GFLOP/s (noisy, sub-ms problem size, but real average uplift).
  Still TODO: V8 tuning sweep; the OpenMP thread-safety fix is in the real
  file even though it was never hand-coded in loop.c.
- `student_gemm_int8.c` — real int8 contract. Second pass applied the same
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
