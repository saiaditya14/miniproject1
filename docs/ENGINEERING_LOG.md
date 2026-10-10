# Engineering log (supporting material for WRITEUP.md)

Sections 6, 7, 8, 10 and 11 of the report in full, kept under the report's section numbers. Section numbers not listed here refer to `WRITEUP.md`.

## 6. Clock, and what we could not fix

The kernel closes at 279-288 MHz against a 300 MHz target. In f1's routed timing the worst 10 paths (-0.242 to -0.231 ns) are one family inside one compute process: pipeline valid bits (`ap_enable_reg_pp0_iterN`, `icmp_ln423_reg`) to the accumulator registers, 3 logic levels, 89-92% of the delay in routing. The generated RTL shows the accumulators update under `if (ap_block == 0 & ap_enable_reg_pp0_iter11 & icmp_ln423_reg_*_iter10 == 1)`; the tool already replicates the condition bit 1,156 times, and the paths are long because the 128 accumulator lanes of one process spread over a large area. The clock rose from 187 to 283 MHz when the accumulator reset net (the earlier worst path, 65k pins) was removed and the compute was split into more processes; the remaining limit is the enable family above.

What did not work, kept so it is not repeated:
- A Vivado physical-optimization pass added to the link step (f4 config): +0.5% clock (279.7 to 281.2 MHz). Placement and routing came out identical to f1; only the post-route pass ran.
- `style = frp` (free-running pipeline) on the compute loops: HLS disables it for this loop (warning HLS 200-1975). Earlier I recorded "frp did not help"; the correct statement is that it was never applied. The pipeline stall signal is constant 0, so frp would have changed little.
- A cross-call weight cache, and other ideas in `ideas2.md`: not built.

RT=10 / RT=12 arrays (f3c, f2c): both failed to route, so no clock is available for them; see section 10.

## 7. Debugging: the loops, the method, and the wrong turns

Every problem below was worked the same way. (1) Turn the symptom into a number: cycles per tile, nanoseconds per row, GB/s, DSPs per module. (2) Build the cheapest experiment that separates two explanations. (3) Read the artifact itself (the routed timing report's worst paths, the generated Verilog, the csynth log, the per-case table from the card) and not a summary of it. (4) Write down what was wrong, including my own earlier conclusions (`results/logs/variants.md` keeps the retractions in order).

| symptom | first explanation | what the evidence showed | fix and result |
|---|---|---|---|
| Portal rejects the dataflow design: wrong results at 32000x288x32 (id 76); cosim at 8 rows passed | a memory-timing effect | Generated RTL: the tile result left the compute stage as 512 scalar return values wired from free-running adders into 2-deep FIFOs; a slower store let the FIFOs fill and a late capture took garbage. Also explained the zeroed 4th/6th tile in RT=12/16 cosim | result goes through a RAM channel with a row-by-row reduction; cosim passes at 64 and 128 rows; the design passes on the card (id 84) |
| Kernel clock stuck at 187 MHz (v5bc, v5bcf); a free-running-pipeline pragma changed nothing | the pipeline stall net | Routed timing report: the worst path (-2.002 ns) was one accumulator reset net with about 65,000 sinks through a clock buffer, 96% routing | no accumulator reset, compute split into processes, epilogue II fixed: 283 MHz (+51%); the pragma was never applied (see section 6) |
| Splitting the compute process doubled the DSP count (2,947, 99% of one SLR) | the split itself | Per-module csynth: the epilogue at II=1 needs 224 adders per cycle, more than a smaller process owns, so the tool bought its own | epilogue II = ceil(7 / rows per process): epilogue DSPs 386 to 2 |
| B-tile load 2,920 cycles per call | slow HBM | csynth log: B reads not burst-inferred; 16 outstanding x ~80-cycle latency = 5 cycles per word | burst length 8, 64 outstanding: 1,072 cycles per call, square -6% |
| N=1 stuck at 6.7 ms across three redesigns | "A reads cap at 6-7 GB/s" | Four stripped-down N=1 kernels (section 3) read A at 12.5 GB/s; then the full design's RTL showed single-beat C writes | write C in 512-bit words: N=1 6.7 to 3.26 ms, tok/s 70 to 104 |
| Vivado physical-optimization pass added to the link step | it would fix the clock-enable net | Routed timing report identical to the baseline; only the post-route pass ran | +0.5%; recorded as a negative result |
| Disk quota threatened by six concurrent builds; five were relaunched with their work directory on node-local `/tmp` | `/tmp` is not charged to the quota | all five failed in the first synthesis step: `ERROR: [VPL 8-5809] Error generated from encrypted envelope` reading the platform's encrypted IP under `/tmp/...` | the flow only works from the home directory; the five builds were resubmitted there, and the quota is managed by limiting concurrent builds instead (about 12-17 GB with three running) |

Things that held across all of these:
- **Cosim is not the card.** It predicts the tiled path to within 1-4% on hardware, but its memory model hides HBM effects: the streaming gemv passed cosim and ran slower on the card, and pre-run estimates ran 1.3-1.7x optimistic.
- **The portal is a measuring instrument.** It times every row of a rejected run, which turned "kernels that only do N=1" into a cheap probe.
- **Explanations I had to withdraw:** "N=1 is time-limited at 6.9 GB/s" (it was cycles per word, a clock problem for one build and the write path for another); "frp did not help" (it was never applied); a tok/s formula fitted on other entries (it over-predicted by 11% for us); "+2% is noise" for f4 (four repeat runs showed it was real). A 24 GB memory request that killed a build was measured, not guessed, afterwards (cgroup resident memory: 15-19 GB in the survivors).
- **Operations matter:** harvest reports before deleting a build folder, keep one folder per variant, and check the scheduler's start estimate (`sbatch --test-only`) before choosing a request size.

## 8. Reproduce

Source for every variant is in `rce_upload/` (one folder per variant, named in `results/logs/variants.md` with job ids and flags). Build and submit commands are in `REPRODUCE.md`. Local correctness gate without Vitis: `g++ -O2 -std=c++17 -Wno-unknown-pragmas -o tb fpga/src/kernel_tb.cpp fpga/src/kernel.cpp && ./tb full`.

## 10. Next architecture: K-unrolled accumulation, and why the bigger arrays failed (in flight)

**Two bigger arrays failed to route.** The RT=10 (f3c, 320.5K LUT by csynth) and RT=12 (f2c, 374.7K LUT) builds both stopped before routing with `ERROR: [VPL 35-3] Design is not routable as its global congestion level is 7`. Their placed utilization shows why: the whole kernel sits in SLR1 (f3c: 304K LUT, 70% of SLR1, 1,723 DSP; f2c: 349K LUT, 81%, 2,041 DSP) while SLR0, which holds the HBM side, has only the shell's 70K LUT (16%) and 4 DSP. The placer kept the whole kernel in one SLR and never used the other. The Vitis documentation (UG1700) says a kernel too large for one SLR is placed across SLRs automatically, and Vivado's `SSI_BalanceSLRs` placement directive asks the partitioner to spread cells evenly; neither happened in our default flow, which is what the next builds test. (I first wrote that a single compute unit cannot span SLRs; that was wrong.) f1 (217K LUT, about 50% of an SLR) routed; 320K did not. I skipped the csynth gate for these two builds, and the gate would have shown the size.

**What a MAC really costs.** Fitting the csynth totals of five designs (f1, f3c, f2c, and the two gated variants below) gives LUT ~ 86K + 28K x (compute processes) + 0.29K x (MACs), within 3% on all five. This is a three-parameter fit to five points, not a validated model. Its content is that each extra compute process costs about 28K LUT of overhead (epilogue, control, B-tile copy) while each added MAC costs about what its cores cost (a 78-LUT multiplier and a 198-LUT adder). Floating-point adders in fabric would cost 338 LUT instead of 198 LUT plus 2 DSP, so they move in the wrong direction when LUTs are the binding resource.

**The K-unrolled loop.** Each output element consumes `KUN` consecutive k-values per cycle (`KUN` multipliers, a small adder tree, one accumulate), so the same MAC count needs fewer outputs, and so fewer accumulator chains and enable-net loads, per process. `KUN=1` reproduces the current design. The gate showed that the saving I expected did not appear in LUTs: k4a is 292K LUT, not the 225K I predicted, because the glue turned out to scale with the number of compute processes, not with the number of outputs. Gated and tested: full testbench plus 1,120 edge shapes (M 1-129, K 16-768, N 2-70) against a double-precision reference, for every variant below.

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
| k4a | 512 MACs (RT=4, CP=2, KUN=4) | default | done: routed, 234.6 MHz, 9/9 pass, slower than f4 (see below) |
| k2d | 384 MACs (RT=6, CP=2, KUN=2) | default | done: routed at 262.3 MHz (job 105026, 5 h 51 min); 9/9 pass; selected design (run 191: 6.436 / 3.227 / 5.281 ms / 107.78 tok/s) |
| b4a | 512 MACs | + `SSI_BalanceSLRs` | running (job 105027) |
| k4s | 512 MACs | + `SSI_SpreadLogic_high` | running (job 105052) |
| sk4 | 512 MACs + persistent streaming A reader for the tiled path | default | locally gated (testbench, 1,359 edge shapes); csynth gate passed (295K LUT, 2,686 DSP, 871 BRAM18, main loops II=1, estimated clock 3.491 ns, same as k4a); RTL cosim PASS (job 105124); hardware build running (job 105123) |
| b6 | 768 MACs | + `SSI_BalanceSLRs` | not built; held until the A-reader result (k4a showed that more MACs alone do not help) |
| b8 | 1,024 MACs | + `SSI_BalanceSLRs` | not built; same |

(The restarts are needed because a first attempt to keep the build directories on node-local `/tmp` failed: the platform's encrypted IP cannot be read from there; see section 7.)

**k4a, measured (retracting the prediction).** I predicted about 1.3x on N=32 and 1.4-1.6x on square over f1 for a 384-512 MAC array at about 280 MHz. k4a routed (5 h 27 min build, 292K LUT, 2,689 DSP) and passed all nine cases on the card, but the kernel clock was scaled to 234.6 MHz (and hbm_aclk from 450 to 403.3 MHz; f4 kept 450). Three submissions (ids 166-168): square 7.737-7.812, N=1 3.268-3.359, N=32 6.215-6.315 ms, tok/s 97.8-101.7; medians 7.780 / 3.336 / 6.271 ms / 100.8. Against f4 that is 0.92x on square, 0.97x on N=1, 0.86x on N=32, so it was not selected.
- **Clock.** Routed WNS -0.929 ns (1000 / (3.333 + 0.929) = 234.6 MHz), 111,271 failing endpoints. The ten worst paths are one family: the B tile (`Bbuf_N`) memories, written from `load_b_tile` and read from `tl_compute`, through a LUTRAM (`RAMS64E`) with 5 logic levels and 84% of the delay in routing. With KUN=4 the B tile is split into 2 x 32 x 4 = 256 small memories, which the tool implements as distributed RAM, and one address and data fan-out drives all of them. The next family is at -0.257 ns (an AXI write pipe, about 278 MHz), so removing the B-tile family would leave a clock near f4's 281 MHz, not above it.
- **Cycles.** Square took 7.78 ms x 234.6 MHz = 1.83M cycles, against 0.89M for 512 MACs fully busy: 48% busy. f4 was 88% busy. Doubling the MACs cut cycles by only 9% (1.83M against 2.01M).
- **Inference (not measured per stage): the limit is now how fast A reaches the array.** At NT=32 each loaded A element is used for 32 columns, so a design with M MACs needs M/32 floats of A per cycle: 8 for f4's 256 MACs (9.0 GB/s at 281 MHz) and 16 for k4a's 512 (15.0 GB/s at 234.6 MHz), against 12.5 GB/s that one channel gave the gemv. Dividing the A bytes of each case by the measured time gives the effective A rate of the tiled path: square (56.6 MB with A re-read per column tile) 7.9 GB/s for f4 and 7.3 for k4a; the N=32 sum (39.0 MB) 7.2 and 6.2 GB/s; against 12.0 GB/s for the N=1 sum with the streaming reader. If that holds, more MACs cannot help the square and N=32 columns until the tiled path reads A at the gemv's rate; the roofline for this tiling is 12.5 GB/s x 16 flop per byte = 200 GFLOP/s, and f4 sits at 63% of it. Untested.

Predicted, not measured, for b6 and b8: square about 3.0 and 2.2 ms with N=32 near 3.2-3.3 ms. These predictions assumed compute-bound square, which the k4a result contradicts; treat them as upper bounds on speed until the A-rate question is settled.

## 11. Next steps (continuing past the 9 October deadline)

Goal: a measured, defensible speedup on a real model that goes beyond the single-channel ceiling of section 4, with the roofline argument extended to the on-chip and multi-bank cases.

**Step 0: choose the base architecture.** f4 stays the base until something beats it on the card. k4a (512 MACs) routed at 234.6 MHz and was slower (section 10); k2d, k4s and b4a (section 10) are still building, and sk4 (streaming A reader on the k4a array) waits for cosim and a hardware build. Rule: adopt a design only if it beats f4 on the columns that matter; among designs that do, take the one with the most MACs that clocks at about 270 MHz or better, and the one with fewer LUTs if two tie. The bigger arrays (b6, b8) wait for the A-reader result, because k4a showed that A delivery, not the MAC count, binds next; after that, fabric adders and a LUT diet (the glue per compute process, about 28K LUT, is the target).

**Step 1: on-chip weight cache inside the fp32 contract.** The grader's host passes each weight matrix as an HBM buffer and calls the kernel once per matrix; a cache keyed by address, with a few sampled words of A as a content signature, can serve repeated calls from URAM and BRAM. Measured constraint: the correctness gate is 1e-2, and weights stored as int8 miss it by 7-15x (worst error 0.075-0.154) and bf16 by 5-13x, while **fp16 passes (worst 0.0029-0.0081, thin margin at K=768)**. fp32 weights (60.8 MB) do not fit in the roughly 28.5 MB of URAM plus BRAM; fp16 weights (30.4 MB) nearly do, so about 7% would still stream from HBM. Needs a wider gemv engine (the current one consumes 16 floats per cycle, about 18 GB/s). Estimate, not a result: 165-185 tok/s against 105 now; the harness floor (about 3.2 ms of host time plus about 1.6 ms of per-call cost per token) caps any kernel near 208 tok/s. The int8 and fp16 checks above came from a NumPy test on random matrices in [-1, 1], one seed.

**Step 2: own host, own model (needs the host-bundle flow).** With control of the host, int8 weights are allowed (the Part 3 sweep: int8 and even 4-bit stay coherent; 2-bit breaks). stories15M in int8 GS 32 is 17.1 MB plus about 3.5 MB of KV cache, so the whole model fits on chip and tok/s stops depending on HBM. For larger models, stream weights from several HBM pseudo-channels at once (32 channels, 316 GB/s measured peak for the whole card per the datasheet, 201 GB/s nominal; we use one at 12.5 GB/s): eight banks would be about 100 GB/s (8 x 12.5, an extrapolation), which on paper puts stories110M in int8 at roughly 1 ms per token (an arithmetic estimate, not a measurement; the published HLSTransform figure for the same model is 57 tok/s on a VU9P). Earlier evidence that bank mappings written into an xclbin are honored by the grader's host: variant `le` (A, B, C on separate banks) ran correctly. Splitting one weight matrix across banks still needs the host to allocate the pieces.

**Step 3: measurement and what a paper would still need.** (a) A power reading during a run (`xbutil`) for tok/s per watt, the headline of the closest published workshop paper; (b) baselines on the same model on CPU and, if available, GPU (we have the laptop CPU: 117 tok/s fp32 and about 307 int8 for stories15M); (c) ablations: bank count, cache on and off, array size. Correctness and cycle counts can be checked without the card with `make hw_emu` and cosim; HBM throughput numbers need the real card. **Open question for the professor:** is the older flow still accepted, where the student's host file is submitted together with the xclbin (`make-submission.sh` in the starter)? It decides whether Steps 1-2 can be measured on hardware or only argued.

**Contributions that stand on current evidence:** the shared-bundle write pitfall (N=1 6.7 to 3.26 ms), the roofline probes (reads of A at 12.5 GB/s from one HBM channel), the per-token budget, the routability findings (SLR1 confinement and the LUT-per-process model). Weight-resident mixed precision under an fp32 contract (Step 1) would be the new result.

