THE MAIN SPOT

Fair question, and yes. So far I've been bringing up a base design and getting it through the toolchain. Design decisions are only worth debating once there's a working flow and real numbers (II, timing, utilization), and until now we had none. I've also been picking defaults quietly, which I should have flagged. The ones I made, and why:

Row-wise outer-product tile with the B tile in BRAM. This is the lecture's winning dataflow (S3/S4), so it was the lowest-risk start.
A separate GEMV path for N=1. N=1 is bandwidth-bound and N>1 is compute-bound, the same split that worked on the CPU.
Float32 with partial-sum accumulators. The tool's fadd is 7 cycles, so we need ≥7 partial sums.
Now that the base is nearly validated, these are real choices, and I'd rather make them with you:

What are we optimizing first? These pull in different directions:

The leaderboard ranks by the square 768³ case, which is compute-bound, so it wants more MAC lanes.
tok/s is dominated by the 43 N=1 calls per token, which are bandwidth-bound and sensitive to per-call overhead.
"Close to roofline" is the easiest to prove for N=1 (port bandwidth) and the hardest for the square case.
My recommendation: scale the tile for the square case first (we're at 12% DSP, so there's lots of headroom), then fix N=1 with A double-buffering so loading overlaps compute.

How aggressive on frequency? We target 300 MHz. Raising the kernel clock (400 MHz or more) lifts both the port-bandwidth and compute roofline by that factor, but needs an edit to the Makefile's link step. I don't know whether the grader accepts any bitstream you build, so ask the instructor before betting on it.

Are numerical tricks allowed? fp16/bf16 multiplies with fp32 accumulate would use far fewer DSPs, and the 1e-2 tolerance probably permits it. But it saves compute only, not bandwidth, since A still arrives as float32, and it risks the story quality. I'd leave it off the table for now.

Memory topology. The link step lets us map A, B and C to different HBM banks and move to ap_uint<512> ports. Both are cheap to try and I'd just run them as experiments.


MEMORY HIERARCHY


---
USER'S IDEA (kept here so it survives sessions): MEMORY HIERARCHY IS THE OBVIOUS WIN
Pull data through the hierarchy on purpose: HBM -> URAM/BRAM -> registers/LUTRAM, use every on-chip resource.

Mapping for the current design
- HBM, one 512-bit port (19.2 GB/s at 300 MHz): A streamed, B, C.
- URAM/BRAM: B tile (K x NT, needs NT floats per cycle), A-tile ping-pong buffers, optionally a whole-A copy.
- LUTRAM/registers: partial-sum accumulators, the current A word and B row.

Concrete directions (not implemented yet; ordered)
1. Within-call residency of A in URAM for large N (square 768^3: A = 2.36 MB, re-streamed 24x = ~57 MB from HBM
   today; load once -> ~2.4 MB). Safe (data lives for one call), directly helps the leaderboard (square) metric.
2. Bigger tile to cut A re-streaming and raise compute: R=16, NT=32 (~512 MACs, ~3000 DSP); B tile in URAM when
   NT grows (1024-bit-wide reads need many banks).
3. Dataflow ping-pong of A tiles so load/compute/store overlap (v5, behind USE_DATAFLOW=1).
4. Cross-call on-chip weight cache keyed by the A address (high ceiling for N=1/tok/s, capacity ~20-29 MB of
   the 60.8 MB weights). RISKY: stale data if the grader reuses an address, unknown whether the kernel stays
   loaded across calls. Late experiment, flag, default off, guard with sampled-fingerprint checks.

Open decisions for the user
- What to optimize first: square (leaderboard, compute-bound) vs N=1 (tok/s, bandwidth-bound).
- Frequency: raising the kernel clock needs a Makefile link-step edit; only if we decide to.
- Numerical tricks (fp16 weights) are off the table for now (saves compute only, risks the 1e-2 tolerance and story).


---
ROOFLINE FRAMING AND TARGETS (see CLAUDE.md "Goal, framing and targets")
Prof's offer: pick any problem; convince him via roofline analysis + real-world arguments that the achieved GFLOP/s are
near the top of what he expects -> A. Main project stays the graded fp32 GEMM (only measured-hardware route).
Compute roof ~600 GFLOP/s (5-DSP fp32 MAC, ~1000 MACs, 300 MHz); N=1 port roof 19.2 GB/s (~9.6 GFLOP/s).
Round 1 target ~250 GFLOP/s on square (512 MACs at 80-85%); stretch 400+ with fabric adders / ~1000 MACs.
Extra levers to try for MAC budget: BIND_OP fadd impl=fabric, fmul impl=meddsp (fewer DSP per MAC), higher clock.

---
KEY OBSERVATIONS AND PROPOSED NEXT STEP (written 2026-10-08 17:50 IST, from e2 routed timing; results below can change this)

Observation 1 - what limited the clock, layer by layer (measured):
  v5bc/v5bcf (RT=8, one 256-MAC process): worst path -2.002 ns = one accumulator RESET net (ap_loop_init_int -> sh reg R pins,
  ~65k pins, through a BUFGCE, 96% routing) -> 3.333+2.002 = 5.335 ns = 187.4 MHz. frp did not change it.
  e2 (RT=8, CP=2, ZINIT=1 = no accumulator reset, streaming gemv, epilogue II fix): final routed WNS -0.194 ns, hold met
  -> ~3.527 ns = ~283 MHz (+51%). The remaining worst paths are inside our kernel (tl_compute), NOT the shell:
  source = pipeline valid bits (ap_enable_reg_pp0_iter11, icmp_ln411_reg_..._pp0_iter9/10), dest = accumulator regs
  (empty_*, p_load*), 3-4 LUT levels, 3.1-3.4 ns, 89-92% routing. = the valid-bit enable/hold net, ~1.1k loads per process.
  (Full report: logs/e2_timing_routed.rpt, 5,214 failing endpoints, TNS -329.)

Observation 2 - resources (csynth, per-module):
  FP32 MAC = 5 DSP (3 fmul + 2 fadd). The compute split duplicated the epilogue adders per process (129-193 fadd x2 DSP)
  because the epilogue at II=1 needs 224 adders/cycle; fix = epilogue II = ceil(7/RTP) so it shares the MAC adders (epilogue DSP 386 -> 2).
  e2: 1413 DSP / 224K LUT / 814 BRAM18 = 47% / 51% / 60% of one SLR. RT=16 (e3) = 2693 DSP (90%), 399K LUT (91%): does not fit one SLR.
  LUT cost ~625 LUT per MAC (float-core glue); 512 MACs is the limit of one SLR.

Observation 3 - cycle model, calibrated against the card (portal ids 100/84/58, logs/runidk.md):
  tile = K + 13 (main loop depth) + epilogue. Epilogue chain 63 cyc -> tree epilogue 29 cyc: 364 -> 335 cyc per 8-row tile (e2c, cosim).
  B-tile load 2920 -> 1072 cyc per K=288 call with the B port set to outstanding=64, burst=8 (cause: B reads were not burst-inferred,
  16 outstanding x ~80 cyc latency = 5 cyc/word). Hardware vs cosim: tiled path +1..4%, old gemv +12%.
  32000x288 is ~94% of the N=1 sum and ~93% of the N=32 sum. N=1 is HBM-port bound (~10-12 GB/s per m_axi port), so tok/s is nearly
  clock-independent above ~190 MHz; N=32/square scale with MAC count x clock.

PROPOSED NEXT STEPS (in order; decide after the e2/e2b/e2c portal numbers)
  1. e2d = e2c with CP=4 (RTP=2): halves the per-process enable net (580 vs 1156 loads). DSP should stay ~1.41K now that the epilogue
     duplication is gone (verify with the csynth gate: DSP ~1.4K, LUT ~51%). Expect ~285-300 MHz.
  2. RT=12 with 6 processes of 2 rows (needs CP=6 support in gemm_tiled): ~384 MACs, est. ~2.05K DSP / ~71% LUT / ~70% BRAM of one SLR.
     Model at ~283 MHz: N=32 ~3.6 ms, square ~4.6 ms (leaderboard rank 3-4). Do this only after step 1 shows the enable-net fix works.
  3. B-tile load further (1072 -> ~600 cyc/call; strided square case still ~2.8k cyc per column tile).
  4. Later: A resident in URAM for square, higher tile efficiency (13 cyc main-loop depth + epilogue still ~47 cyc per tile).
  Rules kept: csynth gate before any hardware build (DSP<95%, LUT<80%, BRAM<85% of one SLR), one change per build, at most 2-3 builds in flight,
  download-before-delete for anything tied to an open issue.

## 2026-10-09: ideas after the f-series and the K-unrolled builds (details in WRITEUP.md sections 10-11)
- On-chip weight cache under the fp32 contract: fp16 is the only compressed format that passes the 1e-2 gate (int8 7-15x over, bf16 5-13x over; fp16 0.003-0.008). fp32 60.8 MB does not fit in about 28.5 MB of URAM+BRAM, fp16 30.4 MB nearly does. Cache keyed by (address, M, K) plus sampled-word signature. Needs a wider gemv engine. Estimate 165-185 tok/s; harness floor about 208.
- Own host: int8 weights allowed; stories15M int8 17.1 MB + 3.5 MB KV fits entirely on chip. Bigger models: stream from several HBM banks in parallel (we use 1 of 32 pseudo-channels). stories110M int8 on 8 banks about 1 ms per token on paper.
- The portal's older flow (student host file bundled with the xclbin) is the way to measure it: ask whether it is still accepted.
- Routability: single CU can span SLRs (UG1700); `SSI_BalanceSLRs` placement directive is the lever (builds b4a/b6/b8). Per-process overhead about 28K LUT; fabric adders only help once LUTs are available in both SLRs.
- Paper-shaped checklist: power (xbutil), CPU/GPU baselines on the same model, ablations, one clear claim.

## Square 768^3 at about 1.8-2.1 ms (fp32), written 2026-10-10. All numbers are estimates from measured inputs; nothing here is built.

Where square stands: k2d (384 MACs, 262.3 MHz) 6.436 ms = 141 GFLOP/s = 70% of its array peak. f4 7.14 ms, k4a 7.78 ms.

Why the current tiling caps out: with 32 columns per tile every column tile re-reads all of A from HBM, 24 times for N=768, 56.6 MB. One channel gives 12.5 GB/s, so the tiling has a 4.5 ms floor whatever the array. A bigger array alone does not help (k4a: 512 MACs, 48% busy). Square's operands are small: A, B and C are 2.36 MB each, and URAM holds about 22.5 MB (A needs about 66 of 640 URAMs).

Target design (a dedicated square path; the N=1 and N=32 paths stay, all nine cases must still pass):
1. Load A and B once, each on its own HBM channel (bank mapping in the xclbin is honoured; variant le ran that way), about 0.2 ms, and keep both on chip in URAM. Safe under the contract: it only holds data within one call.
2. A 2D output-stationary systolic array, for example 32 x 32 PEs, neighbour-only data movement (A passes right, B passes down), each PE interleaving 8 C elements to cover the 7-cycle fp32 add. Built as hls::stream-connected PE processes of small chains; static loop bounds for 768. This avoids the large fanout nets (B-tile banks, enable nets) that limited every clock so far.
3. Place the array across both SLRs by construction (two halves of the rows, B passed across through registered stages). The placement directives SSI_BalanceSLRs and SSI_SpreadLogic_high did nothing for us (b4a 233.1 and k4s 238.0 MHz against k4a 234.6).

Budget: fp32 MAC = 5 DSP (3 multiply, 2 add), so at most about 1,190 MACs (5,952 DSP). LUT fit: LUT ~ 86K + 28K x compute processes + 0.29K x MACs. At about 1,150 MACs and 255 MHz: peak 586 GFLOP/s, 85% utilisation about 500 GFLOP/s, compute about 1.8 ms; load and store (about 0.4 ms) partly overlapped gives about 1.8-2.1 ms. That would be 8th place to about 4th on square at the 22:26 board (5th place was 3.82 ms).

Risks: no array above 512 MACs has closed timing (RT=10 and RT=12 failed to route; b6 and b8 passed csynth only); builds take 6-7 hours; the square path shares area with the paths that remain.

Variant for sub-1 ms (not needed for 2 ms): reduced-MANTISSA multipliers. Checked 2026-10-10 (mant_check.py, logged in logs/variants.md): keeping 14 explicit mantissa bits on both operands (fp32 exponent, fp32 accumulate) passes the 1e-2 gate with 14x margin on uniform[-1,1] and 3.9x on normal(0,1); 16 bits gives 49x and 15x. fp16 on both operands fails (1.1e-2 to 1.3e-2) because of its 10-bit mantissa. A 15-18 bit significand multiply fits one DSP48E2 (27x18) instead of 3, so a MAC costs 3 DSP and the cap becomes about 2,000 MACs (LUTs then limit). It needs a hand-written multiplier in HLS (split exponent and integer mantissa, integer multiply, fp32 add). The 0.986 ms leaderboard entry (919 GFLOP/s, about 1,500 MACs at 300 MHz) is consistent with something like this; that is inference, not knowledge.

Steps in order, each in its own folder and its own build:
1. fpga_sk2d: k2d plus the persistent streaming A reader (sk4's kernel.cpp with RT=6, KUN=2). Predicted N=32 about 3.9-4.2 ms and square about 5.2-5.6 ms at 262 MHz. Read sk4's routed worst path first (sk4 closed at 123.4 MHz; its cycles were 30% lower on square and 19% lower on N=32 than k4a).
2. fpga_k2d_b: k2d with the B-tile memories forced into BRAM or with registered address and data (the worst-path family in k4a's report). Target about 280 MHz, about 8% on square and N=32. Needs a csynth gate for BRAM.
3. Resident A for square on k2d's array (URAM, within one call): about 5.0-5.4 ms.
4. b6 or b8 (768 or 1,024 MACs, already through csynth) with resident A: about 2.6 or 2.0 ms ideal-ish, if the clock closes.
5. The systolic square path above.
6. Separate, for tok/s: a null-kernel probe (no memory access, timed on the portal) to learn how much of the 30 us per call is reachable; the leaders' rows imply 6-14 us per call against our 30.

### General lesson from the GEMM builds (written 2026-10-10)
The lever that moved the compute columns was squeezing more MACs per cycle: K-unrolling took the array from 256 MACs (8 rows x 32 columns x 1) to 384 (6 x 32 x 2) and square fell 10% (7.14 to 6.44 ms). Each step gave less than the MAC count promised, because cycles fell less than the MACs rose (k2d: -16% square cycles for +50% MACs) and the clock fell with size (281, 262, 234 MHz at 256, 384, 512 MACs). The fixed costs that did not shrink were the per-tile restart of the A read, the pipeline and epilogue of every tile, and the B-tile memories that limited the clock. So: more MACs first, then fix whatever stops them from being fed (the streaming A reader, resident operands) and whatever stops the clock (the B-tile memories); and the DSP cost per MAC (5 for fp32, 3 with a reduced-mantissa multiplier) sets how far the MAC count can go.
