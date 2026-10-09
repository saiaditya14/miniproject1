# Mini-Project 1: a GEMM kernel on the U50, and the language model that runs on it

Single bitstream, `extern "C" void gemm(A, B, C, M, K, N)`, float32, U50 (XCU50, HBM2). Everything below is a
measured number from the grader's card unless it says "estimate". Run ids refer to the submission portal; the
raw per-case logs are in `logs/`, the variant table is `logs/variants.md`.

## 1. Summary

| | kernel clock | square 768^3 | N=1 total (4 shapes) | N=32 total (4 shapes) | TinyStories tok/s |
|---|---|---|---|---|---|
| v3 first working design (id 58) | 279 MHz | 32.70 ms | 8.80 ms | 24.03 ms | 57.2 |
| e2b, clock fixed, B-tile load fixed (id 114) | 288 MHz | 7.27 ms | 6.68 ms | 5.72 ms | 70.0 |
| f1, N=1 path fixed (id 138) | 279.7 MHz | 7.29 ms | 3.26 ms | 5.57 ms | 104.1 |
| **f4, four identical submissions (ids 146-149), median** | 281.2 MHz | **7.16 ms** | **3.23 ms** | **5.40 ms** | **105.5** |

The four f4 runs are the same bitstream: square 7.140-7.167, N=1 3.225-3.277, N=32 5.393-5.442, tok/s 103.7-106.6.
The spread is 0.4-1.6% on the kernel scores and 2.8% on tok/s. Run 149 is the one selected for the leaderboard (best of four).

Pending at the time of writing (builds in flight): RT=10 (f3c) and RT=12 (f2c) arrays, which target the N=32 and square
columns. Their numbers go in section 6 when they return.

Leaderboard position: to be filled from the snapshot `leaderboardcopy_at_10_48AM_9th_October.pdf`. Against the nine rows
visible in the 8 October screenshot, the f4 numbers sit about 4th on N=1, 5th on square and tok/s, and around 7th-9th on N=32
(estimate, not re-checked against the later snapshot).

## 1b. Part 1: the CPU GEMM (float32 and int8)

`student_gemm.c` and `student_gemm_int8.c`: single core, AVX2 selected at run time, built by the official harness with plain `gcc -O3 -lm` (no `-march`, no OpenMP). Laptop (i7-1260P); timings drift about 2x between identical short runs, so ratios inside one run are the reliable quantity. Full tables and method: `BENCHMARKS.md`.

| float32, GFLOP/s, best of 7 | N=1 | N=8 | N=32 | N=64 |
|---|---|---|---|---|
| range over the four model shapes | 10.6-22.9 | 49.9-82.5 | 59.0-98.1 | 96.3-110.8 |
| the course's reference kernel | 1.6-4.2 | | 3.3-4.4 | |

- Correctness: the official cases 16/16, plus 64/64 over N from 2 to 100, plus 847 random shapes including odd M, K, N against a double-precision reference (worst absolute error 7e-5, tolerance 1e-2). int8 is bit-exact against the reference (relative tolerance 1e-4, which forces the reference operation order `(As*Bs)*dot`) at N = 1, 3, 7, 8, 24, 33, 40, 64, 100.
- Against OpenBLAS 0.3.28, one thread each, N=1 vs `cblas_sgemv` and N>1 vs `cblas_sgemm` (median of 3 runs): 0.97-1.27x at N=1, 1.2-1.8x at N=8, 1.13-1.22x at N=32, 1.06-1.12x at N=64. That is a shape-specialised kernel against a general library on four fixed shapes and N <= 64 only; I would call 1.0-1.1x "about equal".
- Two earlier claims were retracted: a 2.5-3.7x win at N=1 compared against `sgemm` on a vector (an unfair baseline), and a 1.18x classifier win at N=32 that was a 16-thread run.

## 2. Roofline

Two different bounds apply, because the cases are different kinds of work.

**N=1 is bandwidth-bound.** A matrix-vector product reads every weight once and does 2 flop with it: 0.5 flop/byte. The
four N=1 shapes read 38,965,248 B of A in total, 36.9 MB of it in the 32000x288 case.
- One HBM2 pseudo-channel: 450 MHz x 32 B = 14.4 GB/s. One 512-bit kernel port at 300 MHz would be 19.2 GB/s, so the channel is the binding limit.
- Measured on the 32000x288 case: 2.944-2.981 ms, i.e. **12.4-12.5 GB/s, 86-87% of the channel peak**. The whole N=1 sum (38.97 MB in 3.26 ms) is 11.95 GB/s, 83%.
- A probe kernel that only reads A (no multiply, XOR checksum) took 2.948 ms; the real gemv took 2.952-2.981 ms. The gemv is within 1% of the best this port can do.

**Square 768^3 and N=32 are compute-bound.** 906 MFLOP for the square case.
- The array is 256 MACs (8 rows x 32 columns), one fp32 MAC per lane per cycle: 256 x 2 x 281 MHz = **144 GFLOP/s peak**.
- Measured: 906 MFLOP / 7.16 ms = **126.5 GFLOP/s, 88% of the array's peak** (f1: 124.3 GFLOP/s, 87%). On the 32000x288x32 case, 113.8 GFLOP/s, 79%.
- Against the device: with the default float core (5 DSP per MAC: 3 for the multiply, 2 for the add) about 1,190 MACs fit in 5,952 DSPs, a roof of about 710 GFLOP/s at 300 MHz. We use 17-18% of that. The array is well utilised, but it is a small array; more MACs per LUT and per DSP is what the faster entries on the board must have done.

## 3. How N=1 went from 6.7 ms to 3.26 ms

The N=1 sum was stuck at 6.7 ms across three redesigns while square and N=32 improved 35%. The earlier explanation ("A reads cap at about 6-7 GB/s") was wrong. What found the cause:

1. **Isolated probes.** The portal times every row of a run even when the run is rejected for wrong results (id 76 showed it). So N=1-only kernels, which fail the N>1 rows by design, still report N=1 times. Four small builds (86 DSP, 17K LUT instead of 769 DSP, 157K LUT):
   la (streaming gemv) 2.981 ms; le (A, B, C on different HBM banks) 2.952; lb (64 outstanding reads instead of 16) 2.950; pa (pure reader) 2.948. HBM bank placement, outstanding count and the FIFO chain make no difference. The read path was never the limit.
2. **The full design, same gemv code, was 6.35 ms.** The generated RTL of the full design shows the C write loop issuing `AWLEN = 1` single-beat writes, one per output row, and the csynth log has no write-burst line for it; the lite kernel's log does. Cause: the tiled path stores C 16 floats at a time, which widens the whole `gmem2` bundle to 512 bits, so the gemv's 32-bit C writes became strobed single-beat writes. The extra 3.4 ms over the lite kernel across 32,000 rows is about 106 ns, roughly 30 cycles per write (inferred from the time difference, not measured per write).
3. **The fix** is a 23-line change: write C in whole 512-bit words, scalar tail for partial tiles. csynth then reports "burst writes of length 4 and bit width 512"; area is unchanged. RTL co-simulation passes on all seven test cases. On the card (f1): N=1 3.26 ms, equal to the lite kernel; tok/s 77 -> 104.

## 4. What limits tok/s: the per-token budget

A token is 43 matrix-vector products (24 of 288x288, 12 of 768x288, 6 of 288x768, one 32000x288) and moves 60.8 MB of fp32 weights.
From the f1 per-case rows (0.063 / 0.106 / 0.111 / 2.979 ms) the matmuls take 24 x 0.063 + 12 x 0.106 + 6 x 0.111 + 2.979 = **6.43 ms**.
The token takes 1000 / 104.08 = 9.61 ms, so the rest is **about 3.2 ms** of host work. The same arithmetic on older runs gives 3.2 ms
(v5bcf) and 2.8 ms (v3), so it is a property of the host, not of one design.

| part of one token | time | share |
|---|---|---|
| host work between matmuls (norms, attention, sampling, launches) | ~3.2 ms | 33% |
| streaming 60.8 MB of weights at 12.5 GB/s | 4.86 ms | 51% |
| fixed cost of 43 kernel calls (about 36-40 us each: launch plus pipeline fill) | ~1.6 ms | 16% |

The three terms add to 9.7 ms against 9.6 ms measured. This is the throughput ceiling, and it names what raises it:
more bandwidth than one pseudo-channel gives, fewer bytes per token (int8 would cut the 4.86 ms to about 1.2 ms), weights resident on chip
(int8 weights are about 15 MB, which fits in the U50's roughly 28 MB of URAM and BRAM), fusing calls to cut the per-call cost, or overlapping the host work. We did not build any of these.

## 5. Part 2 questions (CPU-side measurements, laptop, plain `gcc -O3`)

- **Matmul fraction f:** 91.8-95.7% of runtime (mean ~94%); the Amdahl cap for accelerating only the GEMMs is about 17x (12-23x).
- **Which shape dominates:** the 288->32000 classifier, 64.9% of matmul time, not the FFN (w1/w3 15.3%, attention 10.3%, w2 9.5%). On the card the classifier is 2.98 of 6.43 ms of matmul per token (46%).
- **tok/s versus N (CPU, aggregate tokens/s, median of 5):** N=1 296, N=2 263, N=4 447, N=8 729, N=16 782, N=32 510, N=64 681. It rises about 2.5x and flattens near 700-800 where non-GEMM work (attention per stream, sampling, gather) dominates. The N=32 dip is an unverified hypothesis (per-stream KV caches falling out of cache).
- **On the card:** the portal runs the story at N=1 only, so a hardware tok/s-versus-N curve cannot be measured. What we have is the kernel itself at both ends: the N=1 total is 3.26 ms and the N=32 total is 5.44 ms. 32 times the work costs 1.7 times the time, so the cost per vector falls about 19x. That is the batching payoff, limited by the same HBM channel when the weights are read once per call.
- **Offload order (CPU backend, tok/s median of 7):** none 118, classifier only 166, plus w1/w3 172, plus w2 248, plus attention 288. The classifier alone is the biggest step.

## 6. Clock, and what we could not fix

The kernel closes at 279-288 MHz against a 300 MHz target. In f1's routed timing the worst 10 paths (-0.242 to -0.231 ns) are one family inside one compute process: pipeline valid bits (`ap_enable_reg_pp0_iterN`, `icmp_ln423_reg`) to the accumulator registers, 3 logic levels, 89-92% of the delay in routing. The generated RTL shows the accumulators update under `if (ap_block == 0 & ap_enable_reg_pp0_iter11 & icmp_ln423_reg_*_iter10 == 1)`; the tool already replicates the condition bit 1,156 times, and the paths are long because the 128 accumulator lanes of one process spread over a large area. The clock rose from 187 to 283 MHz when the accumulator reset net (the earlier worst path, 65k pins) was removed and the compute was split into more processes; the remaining limit is the enable family above.

What did not work, kept so it is not repeated:
- A Vivado physical-optimization pass added to the link step (f4 config): +0.5% clock (279.7 to 281.2 MHz). Placement and routing came out identical to f1; only the post-route pass ran.
- `style = frp` (free-running pipeline) on the compute loops: HLS disables it for this loop (warning HLS 200-1975). Earlier I recorded "frp did not help"; the correct statement is that it was never applied. The pipeline stall signal is constant 0, so frp would have changed little.
- A cross-call weight cache, and other ideas in `ideas2.md`: not built.

RT=10 / RT=12 arrays (f3c, f2c): <fill in clock, square, N=32 when the builds return>.

## 7. Debugging: the loops, the method, and the wrong turns

Every problem below was worked the same way. (1) Turn the symptom into a number: cycles per tile, nanoseconds per row, GB/s, DSPs per module. (2) Build the cheapest experiment that separates two explanations. (3) Read the artifact itself (the routed timing report's worst paths, the generated Verilog, the csynth log, the per-case table from the card) and not a summary of it. (4) Write down what was wrong, including my own earlier conclusions (`logs/variants.md` keeps the retractions in order).

| symptom | first explanation | what the evidence showed | fix and result |
|---|---|---|---|
| Portal rejects the dataflow design: wrong results at 32000x288x32 (id 76); cosim at 8 rows passed | a memory-timing effect | Generated RTL: the tile result left the compute stage as 512 scalar return values wired from free-running adders into 2-deep FIFOs; a slower store let the FIFOs fill and a late capture took garbage. Also explained the zeroed 4th/6th tile in RT=12/16 cosim | result goes through a RAM channel with a row-by-row reduction; cosim passes at 64 and 128 rows; the design passes on the card (id 84) |
| Kernel clock stuck at 187 MHz (v5bc, v5bcf); a free-running-pipeline pragma changed nothing | the pipeline stall net | Routed timing report: the worst path (-2.002 ns) was one accumulator reset net with about 65,000 sinks through a clock buffer, 96% routing | no accumulator reset, compute split into processes, epilogue II fixed: 283 MHz (+51%); the pragma was never applied (see section 6) |
| Splitting the compute process doubled the DSP count (2,947, 99% of one SLR) | the split itself | Per-module csynth: the epilogue at II=1 needs 224 adders per cycle, more than a smaller process owns, so the tool bought its own | epilogue II = ceil(7 / rows per process): epilogue DSPs 386 to 2 |
| B-tile load 2,920 cycles per call | slow HBM | csynth log: B reads not burst-inferred; 16 outstanding x ~80-cycle latency = 5 cycles per word | burst length 8, 64 outstanding: 1,072 cycles per call, square -6% |
| N=1 stuck at 6.7 ms across three redesigns | "A reads cap at 6-7 GB/s" | Four stripped-down N=1 kernels (section 3) read A at 12.5 GB/s; then the full design's RTL showed single-beat C writes | write C in 512-bit words: N=1 6.7 to 3.26 ms, tok/s 70 to 104 |
| Vivado physical-optimization pass added to the link step | it would fix the clock-enable net | Routed timing report identical to the baseline; only the post-route pass ran | +0.5%; recorded as a negative result |

Things that held across all of these:
- **Cosim is not the card.** It predicts the tiled path to within 1-4% on hardware, but its memory model hides HBM effects: the streaming gemv passed cosim and ran slower on the card, and pre-run estimates ran 1.3-1.7x optimistic.
- **The portal is a measuring instrument.** It times every row of a rejected run, which turned "kernels that only do N=1" into a cheap probe.
- **Explanations I had to withdraw:** "N=1 is time-limited at 6.9 GB/s" (it was cycles per word, a clock problem for one build and the write path for another); "frp did not help" (it was never applied); a tok/s formula fitted on other entries (it over-predicted by 11% for us); "+2% is noise" for f4 (four repeat runs showed it was real). A 24 GB memory request that killed a build was measured, not guessed, afterwards (cgroup resident memory: 15-19 GB in the survivors).
- **Operations matter:** harvest reports before deleting a build folder, keep one folder per variant, and check the scheduler's start estimate (`sbatch --test-only`) before choosing a request size.

## 8. Reproduce

Source for every variant is in `rce_upload/` (one folder per variant, named in `logs/variants.md` with job ids and flags). Build and submit commands are in `logs/handoff_2026-10-08_evening.md`. Local correctness gate without Vitis: `g++ -O2 -std=c++17 -Wno-unknown-pragmas -o tb fpga/src/kernel_tb.cpp fpga/src/kernel.cpp && ./tb full`.

## 9. Part 3: int8, and where the story breaks

`part3/int8_backend.c` routes `runq`'s matmul through our `student_gemm_int8` (A = the weight tensor with M = d, K = n; B = the quantized activation vector, N = 1; scale layouts match the contract). Greedy output is byte-identical to the golden `runq` on two prompts, and sampled output (t = 0.8, top-p 0.9, 200 tokens) is identical on 10 of 10 seeds.
Speed on the laptop (greedy, 256 tokens, 5 runs): stock fp32 `run` 115-119 tok/s, golden `runq` 256-324, ours 299-323. Medians 117 / 307 / 323, so int8 is about 2.7x faster than fp32 on the CPU and our kernel keeps up with the golden.

**Group size 64 does not exist for this model.** The model dimension is 288, not a multiple of 64, so the exporter falls back to 32 (its "GS=64" file is the same size as the GS=32 file). The sweep therefore uses group sizes 32, 16 and 8.

**The sweep.** `runq` only does int8, so lower widths are requantized to b bits (range +-(2^(b-1)-1), per-group scale) and stored in the same container. Story quality is exactly that of b-bit weights; memory is computed from the format (b/8 + 4/GS bytes per weight); tok/s is measured only for int8 (sub-8-bit would need a packed kernel, not built). Quantized tensors total 15.19M weights; the classifier (shared with the token table) is 9.22M. fp32 baseline: 60.8 MB of weights, 36.9 MB for the classifier. Greedy decoding, `part3/sweep.py`, raw results in `part3/results/sweep.json`.

| precision | classifier | all weights | story (prompt "Once upon a time", greedy) |
|---|---|---|---|
| fp32 | 36.9 MB | 60.8 MB | "...a big, red ball in the sky. It was the sun! She thought it was so pretty." |
| int8, GS 32 | 10.4 MB | 17.1 MB | same words as fp32 for the first 230 characters |
| int8, GS 16 / 8 | 11.5 / 13.8 MB | 19.0 / 22.8 MB | same (GS 8 diverges a little later) |
| 6-bit, GS 32 | 8.1 MB | 13.3 MB | coherent, different story ("a big, dark cloud ... going to rain") |
| 5-bit, GS 32 | 6.9 MB | 11.4 MB | coherent, different story (park, swings, a scary dog) |
| 4-bit, GS 32 / 16 / 8 | 5.8 / 6.9 / 9.2 MB | 9.5 / 11.4 / 15.2 MB | coherent in all three, different story each time |
| 3-bit, GS 32 / 8 | 4.6 / 8.1 MB | 7.6 / 13.3 MB | still coherent English (a boy named Timmy / a red apple on a tree) |
| **2-bit, GS 32 / 8** | 3.5 / 6.9 MB | 5.7 / 11.4 MB | **breaks**: "Once upon a time firsthingheshesheshesherunchunchunchunch..." and "away always always for, read read read read..." |

The edge is between 3 and 2 bits, at both group sizes. The exact-match measure (share of the fp32 story reproduced before the first divergence, mean over 5 prompts) is harsh and not monotonic: int8 GS 32 keeps 38%, GS 16 48%, 6-bit 13.5%, 4-bit 5-8%, 3-bit 3-5%, 2-bit 2.5%. Greedy decoding diverges at the first near-tie in the logits, so even int8 does not reproduce the whole fp32 story; coherence, not identity, is the practical test. Limits: one model, 5 prompts, one seed per prompt, activations stay int8, and "coherent" is my reading of the text, not a metric.

**Trade-off I would ship.** int8 with GS 32: 3.6x fewer weight bytes (17.1 MB against 60.8 MB), the story matches fp32 for the part we compared, and the golden-matching kernel runs 2.7x faster on the CPU. 4-bit saves another 1.8x of memory but gives a different story every time, which I would accept only if memory were the binding constraint. On the FPGA, int8 would move the weight-streaming term of section 4 from 4.86 ms to about 1.4 ms per token; with the host time (3.2 ms) and per-call cost (1.6 ms) unchanged that is about 6.2 ms per token, roughly 160 tok/s (estimate; no int8 FPGA kernel was built, and 4-bit would give about 178).

## 10. Next architecture: K-unrolled accumulation, and why the bigger arrays failed (in flight)

**Two bigger arrays failed to route.** The RT=10 (f3c, 320.5K LUT by csynth) and RT=12 (f2c, 374.7K LUT) builds both stopped before routing with `ERROR: [VPL 35-3] Design is not routable as its global congestion level is 7`. Their placed utilization shows why: the whole kernel sits in SLR1 (f3c: 304K LUT, 70% of SLR1, 1,723 DSP; f2c: 349K LUT, 81%, 2,041 DSP) while SLR0, which holds the HBM side, has only the shell's 70K LUT (16%) and 4 DSP. A single compute unit cannot spread across the SLR boundary, so the usable budget is what SLR1 allows at a routable density, not the chip. f1 (217K LUT, about 50% of an SLR) routed; 320K did not. I skipped the csynth gate for these two builds, and the gate would have shown the size.

**What a MAC really costs.** Fitting the csynth totals of five designs (f1, f3c, f2c, and the two gated variants below) gives LUT ~ 86K + 28K x (compute processes) + 0.29K x (MACs), within 3% on all five. This is a three-parameter fit to five points, not a validated model. Its content is that each extra compute process costs about 28K LUT of overhead (epilogue, control, B-tile copy) while each added MAC costs about what its cores cost (a 78-LUT multiplier and a 198-LUT adder). Floating-point adders in fabric would cost 338 LUT instead of 198 LUT plus 2 DSP, so they move in the wrong direction when LUTs are the binding resource.

**The K-unrolled loop.** Each output element consumes `KUN` consecutive k-values per cycle (`KUN` multipliers, a small adder tree, one accumulate), so the same MAC count needs fewer outputs, and so fewer accumulator chains and enable-net loads, per process. `KUN=1` reproduces the current design. Gated and tested: full testbench plus 1,120 edge shapes (M 1-129, K 16-768, N 2-70) against a double-precision reference, for every variant below.

| variant | array | MACs | csynth LUT | DSP | main-loop II |
|---|---|---|---|---|---|
| k2a | RT=8, CP=4, KUN=2 | 512 | 356K (81% of an SLR) | 2,685 | 1 |
| k4a | RT=4, CP=2, KUN=4 | 512 | 292K (67%) | 2,689 | 1 |
| k2d | RT=6, CP=2, KUN=2 | 384 | ~255K predicted (58%) | | |

k2a is rejected on size. A second round after reading the Vitis and Vivado documentation (a single compute unit may span SLRs, and `SSI_BalanceSLRs` tells the placer to spread logic evenly across them) gated two larger arrays:

| variant | array | MACs | csynth LUT | DSP | BRAM18 | main-loop II |
|---|---|---|---|---|---|---|
| b6 | RT=6, CP=3, KUN=4 | 768 | 412.6K (47% of the chip, 94% of one SLR) | 3,967 (67% of the chip) | 1,031 | 1 |
| b8 | RT=8, CP=4, KUN=4 | 1,024 | 533.6K (61% of the chip) | 5,245 (88% of the chip) | 1,191 | 1 |

**In progress (builds submitted; results will be added here):**

| build | array | placement flow | state |
|---|---|---|---|
| k4a | 512 MACs (RT=4, CP=2, KUN=4) | default | running |
| k4s | 512 MACs | + `SSI_SpreadLogic_high` | queued |
| k2d | 384 MACs (RT=6, CP=2, KUN=2) | default | queued |
| b4a | 512 MACs | + `SSI_BalanceSLRs` | queued |
| b6 | 768 MACs | + `SSI_BalanceSLRs` | queued |
| b8 | 1,024 MACs | + `SSI_BalanceSLRs` | queued |

Predicted, not measured: about 1.3x on N=32 and 1.4-1.6x on square over f1 for the 384-512 MAC arrays if the clock holds near 280 MHz and they route; for b6 and b8, square about 3.0 and 2.2 ms with N=32 near 3.2-3.3 ms (bounded below by about 3 ms of memory time).

## 11. Next steps (continuing past the 9 October deadline)

Goal: a measured, defensible speedup on a real model that goes beyond the single-channel ceiling of section 4, with the roofline argument extended to the on-chip and multi-bank cases.

**Step 0: choose the base architecture.** Builds in flight (section 10): k4a, k4s, k2d (single-SLR flow) and b4a, b6, b8 (`SSI_BalanceSLRs`, spanning both SLRs, 512 / 768 / 1024 MACs). Rule: take the design with the most MACs that routes with a kernel clock of about 270 MHz or better; if two tie, the one with fewer LUTs. If none routes, f4 stays the base and the bigger arrays wait for fabric adders and a LUT diet (the glue per compute process, about 28K LUT, is the target).

**Step 1: on-chip weight cache inside the fp32 contract.** The grader's host passes each weight matrix as an HBM buffer and calls the kernel once per matrix; a cache keyed by address, with a few sampled words of A as a content signature, can serve repeated calls from URAM and BRAM. Measured constraint: the correctness gate is 1e-2, and weights stored as int8 miss it by 7-15x (worst error 0.075-0.154) and bf16 by 5-13x, while **fp16 passes (worst 0.0029-0.0081, thin margin at K=768)**. fp32 weights (60.8 MB) do not fit in the roughly 28.5 MB of URAM plus BRAM; fp16 weights (30.4 MB) nearly do, so about 7% would still stream from HBM. Needs a wider gemv engine (the current one consumes 16 floats per cycle, about 18 GB/s). Estimate, not a result: 165-185 tok/s against 105 now; the harness floor (about 3.2 ms of host time plus about 1.6 ms of per-call cost per token) caps any kernel near 208 tok/s. The int8 and fp16 checks above came from a NumPy test on random matrices in [-1, 1], one seed.

**Step 2: own host, own model (needs the host-bundle flow).** With control of the host, int8 weights are allowed (the Part 3 sweep: int8 and even 4-bit stay coherent; 2-bit breaks). stories15M in int8 GS 32 is 17.1 MB plus about 3.5 MB of KV cache, so the whole model fits on chip and tok/s stops depending on HBM. For larger models, stream weights from several HBM pseudo-channels at once (32 channels, about 460 GB/s total; we use one at 12.5 GB/s): eight banks would be about 100 GB/s, which on paper puts stories110M in int8 at roughly 1 ms per token (an arithmetic estimate, not a measurement; the published HLSTransform figure for the same model is 57 tok/s on a VU9P). Earlier evidence that bank mappings written into an xclbin are honored by the grader's host: variant `le` (A, B, C on separate banks) ran correctly. Splitting one weight matrix across banks still needs the host to allocate the pieces.

**Step 3: measurement and what a paper would still need.** (a) A power reading during a run (`xbutil`) for tok/s per watt, the headline of the closest published workshop paper; (b) baselines on the same model on CPU and, if available, GPU (we have the laptop CPU: 117 tok/s fp32 and about 307 int8 for stories15M); (c) ablations: bank count, cache on and off, array size. Correctness and cycle counts can be checked without the card with `make hw_emu` and cosim; HBM throughput numbers need the real card. **Open question for the professor:** is the older flow still accepted, where the student's host file is submitted together with the xclbin (`make-submission.sh` in the starter)? It decides whether Steps 1-2 can be measured on hardware or only argued.

**Contributions that stand on current evidence:** the shared-bundle write pitfall (N=1 6.7 to 3.26 ms), the roofline probes (86-87% of one HBM channel), the per-token budget, the routability findings (SLR1 confinement and the LUT-per-process model). Weight-resident mixed precision under an fp32 contract (Step 1) would be the new result.

## 12. Evidence index

Every number above comes from one of these. "Run id" is the grader portal's submission id. Files marked `evidence/` are archives of the build logs, reports and RTL for the runs they name; variant source is in `rce_upload_variants.zip` (one folder per variant, flags in `logs/variants.md`).

| claim | where it comes from |
|---|---|
| v3: square 32.70 / N=1 8.80 / N=32 24.03 ms / 57.2 tok/s | run 58; per-case rows in `logs/runidk.md` |
| e2b 7.27 / 6.68 / 5.72 / 70.0; e2c 7.27 / 6.71 / 5.59 / 64.9 | runs 114 and 116; `evidence/harvest_e2b.zip`, `evidence/harvest_e2c.zip` |
| a rejected run is still timed on every row | run 76 (FAIL on 32000x288x32 only; all nine rows timed), recorded in `logs/variants.md` |
| N=1 probes: la 2.981, le 2.952, lb 2.950, pa 2.948 / 2.944 ms on 32000x288 | runs 122, 123, 126, 125; sources are the `fpga_la`, `fpga_le`, `fpga_lb`, `fpga_pa` folders in `rce_upload_variants.zip`; summary in `logs/variants.md` ("N=1 probe results") |
| the full design's C writes are single-beat (`AWLEN = 1`, 512-bit `gmem2`), no write-burst line in its csynth log | generated RTL in `evidence/rtl_e2_gv.tgz`; csynth log in `evidence/keep_e.zip` (`e2_csyn.log`) |
| f1: 7.29 / 3.26 / 5.57 ms / 104.1 tok/s at 279.7 MHz; the N=1 fix | run 138; routed timing and build log in `evidence/harvest_f1.zip` |
| f4: four runs, medians 7.160 / 3.233 / 5.404 ms / 105.5 tok/s, 281.2 MHz | runs 146-149; `evidence/harvest_f4.zip` (build log, clock line, routed timing report) |
| routed WNS -0.242 ns, worst paths one accumulator-enable family; f4 report identical to f1's | `evidence/harvest_f1.zip` and `evidence/harvest_f4.zip` (`*timing_summary_routed.rpt`); RTL of the loop in `evidence/harvest_f4_rtl.tgz` |
| f3c / f2c fail with "global congestion level 7"; kernel sits in SLR1 (70% / 81% LUT) with SLR0 at 16% | `evidence/harvest_fail.zip` (build logs, `slr_util_placed.rpt`, csynth totals) |
| csynth totals behind the LUT fit (f1 217K, f3c 320.5K, f2c 374.7K, k4a 292K, k2a 356K) and b6 / b8 | `evidence/keep_e.zip`, `evidence/harvest_fail.zip`, and the gate outputs recorded in `logs/variants.md` and section 10 |
| leaderboard positions | `leaderboardcopy_at_10_48AM_9th_October.pdf` (25 rows, 10:48 on 9 October) |
| Part 1 tables, OpenBLAS ratios, retractions | `BENCHMARKS.md`, `student_gemm.c`, `student_gemm_int8.c`, `bench_openblas.c` |
| Part 2 CPU profile, batching curve, offload ladder | `notes.md` (sections "Part 2 profile" and "Part 2, everything before the FPGA"), `part2/` |
| Part 3 seam and sweep | `part3/int8_backend.c`, `part3/build_runq_slm.sh`, `part3/sweep.py`, `part3/results/sweep.json` |
| fp16 passes the 1e-2 gate, int8 and bf16 do not | `part3/precision_check.py` (prints the table in section 11) |
| the per-token budget (6.43 ms matmul + 3.2 ms host) | arithmetic from the per-case rows of run 138 in section 4; the same decomposition for runs 100 and 58 uses `logs/runidk.md` |

Not backed by an archived artifact: the portal's per-case rows for runs 114-149 were read from the portal and quoted in this document and `logs/variants.md`; they remain on the portal under the same ids.
