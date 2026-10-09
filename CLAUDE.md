# Mini-Project 1 (FPGA course): GEMM -> TinyStories on the U50

Course: FPGA-based Accelerator Design (IIIT-H, Monsoon 2026, Prof. Suresh Purini). Work happens in
this repo; the user (a competitive programmer, strong C, new to RTL/HLS) is the owner.

## Working rules (follow these)
- Never run `git commit` / `git push`. The user commits. Creating or editing files is fine.
- No comments in code files unless asked. Docs (md) are fine.
- Be honest with numbers. The user puts claims on a resume. Always compare against the right baseline
  (N=1 vs `cblas_sgemv`, N>1 vs `cblas_sgemm`, OpenBLAS pinned with `OMP_NUM_THREADS=1`). Record
  retractions instead of silently changing history (see BENCHMARKS.md).
- The user wants speed: write the code yourself, discuss design choices and pruning heuristics with
  them. Verify before claiming (run the official testers, read generated assembly/reports).
- PARALLEL EXPLORATION IS WANTED, SEQUENTIAL-ONLY IS NOT. The user explicitly does not want to block on one
  version at a time (this will run to many variants). What made them angry was v5 silently overwriting the
  v4 source they were about to upload. Rule: build several variants at once, but keep each one isolated and
  recoverable (separate flag sets/config headers or separate source files, and one RCE folder per variant,
  e.g. ~/fpga_<name>, so parallel Slurm jobs cannot corrupt each other's build/). Never overwrite the file
  or folder a variant under test lives in. Always say which variant name = which flags, so pasted reports
  can be matched. Order of priority comes from the pasted results, not from a fixed queue.
- Do not volunteer 'ask the instructor' advice for things we are not actually doing. Nothing in the
  course-provided files (testbench, Makefile, cfg, scripts, kernel signature) has been modified.
- Tool quirk on this setup: `\\n` inside Bash heredocs/python snippets gets collapsed to a real newline.
  Build backslashes with `chr(92)` in python when writing C strings.

## Where things are
- `student_gemm.c`, `student_gemm_int8.c`: Part 1 deliverables (CPU, single core, AVX2 chosen at runtime
  via `target` attributes + `__builtin_cpu_supports`; OpenMP pragmas are commented out on purpose).
- `loop.c`: the user's own hand-written V1-V6 (naive, ikj, tiled, packed, 2x2 microkernel, SIMD).
- `BENCHMARKS.md` (results + methodology + retractions), `notes.md` (full running log), `bench_openblas.c`.
- `part2/`: `batch_bench.c` (batched llama2.c forward, N=B), `slm_backend_select.c` (offload only some shapes).
- `fpga/`: the graded HLS project (Makefile, slurm.sh, src/kernel.cpp, src/kernel_cfg.h, kernel_tb.cpp).
- `logs/`: pasted RCE logs (run1.md = csynth of v2, run2.md = csynth of v3).
- `ideas2.md`: the user's 'main spot' for FPGA design ideas. Its original text is an earlier design-choice
  discussion (not yet edited by the user). The user's own idea is the memory hierarchy (HBM -> URAM/BRAM ->
  registers); a section for it is appended at the end. Read it before proposing design directions and keep
  adding ideas there.
- Course material: `mpstatement.pdf`, `lecture/gemm_fpga_lect.pdf`, `mini1-gemm-starter.zip` (Part 1-3
  scripts + reference impls), `mini1-gemm-final-starter.zip` (FPGA kernel starter), `_extract/` (hello-hls).

## Part 1/2 facts
- Official tests build with plain `gcc -O3 ... -lm` (no -march, no -fopenmp), single core, one cold call.
  Float tolerance 1e-2. Int8 tolerance is per-element relative 1e-4 and the reference is bit-exact, so the
  int8 kernel must keep the reference float op order `(As*Bs)*dot`, accumulating groups in ascending order,
  and must be compiled with `target("avx2")` only (no FMA contraction).
- int8 scale layout (confirmed): `As[i*(K/GS)+g]`, `Bs[g*N+j]`.
- Status: float passes official + 847 random shapes; int8 bit-exact at many N. Single-thread vs OpenBLAS:
  N=1 0.97-1.27x, N=8 1.2-1.8x, N=32 1.13-1.22x, N=64 1.06-1.12x (shape-specialised, N<=64 only).
- Part 2 CPU side is done (profile: matmul f~94%, classifier 65% of matmul; batched curve; per-shape
  offload ranking). Only the FPGA part of Part 2 remains.
- llama2.c + stories15M.bin were cloned into the session scratchpad (volatile). Rebuild with
  `-w -Wno-error=incompatible-pointer-types -Wno-error=implicit-function-declaration` plus `win.c` on
  Windows/GCC 14.

## FPGA task (the current focus)
Goal: top of the leaderboard AND as close to the roofline as possible. The board can be sorted by ANY of the four columns (square 768^3 ms, N=1 total ms, N=32 total ms, tok/s) and the user weighs all four equally: never say or assume that rank is by square (the user has corrected this across many sessions).
The professor's offer (relayed by the user): pick ANY problem statement; if a roofline analysis plus
real-world arguments convince him the achieved GFLOP/s are near the top of what he expects, he will sign an
agreement giving an A in the course. So achieved GFLOP/s measured on hardware is the currency.
- Contract: `extern "C" void gemm(const float *A, const float *B, float *C, int M, int K, int N)`, m_axi
  bundles gmem0/1/2 (depth= must equal COSIM_*_ELEMS in kernel_tb.cpp: 6144 / 11520 / 256). One bitstream
  must handle (M,K) in {288x288, 768x288, 288x768, 32000x288} at N=1 and 32, plus 768^3.
- Scoring: 9 cases all within 1e-2, then kernel ms for square / N=1 sum / N=32 sum, then a 256-token
  TinyStories run -> tok/s. Placeholder 2 tok/s, simple tiled ~7.
- Workflow on RCE (never on the login node): `sinteractive -c 8 -A cs2401`; `make csim` (~20 s),
  `make csynth` (~2 min), `make cosim` (minutes), `./slurm.sh hw` (~1 h, `build/hw/gemm.xclbin`),
  `HW_TOKEN=<token> make submit` from the login shell. Host `rce.iiit.ac.in` needs IIIT VPN. Never store
  the HW_TOKEN in the repo. Only the user can run RCE; paste reports back into the chat.
- Local correctness gate (no Vitis needed): `g++ -O2 -std=c++17 -Wno-unknown-pragmas
  -o /tmp/tb fpga/src/kernel_tb.cpp fpga/src/kernel.cpp && /tmp/tb full` plus an edge-shape test.
- Current kernel (v3): two paths. N=1 `gemv`: 16 floats/cycle from 512-bit A reads, 8-row tile, balanced
  16-way adder tree, accumulators rotating over rows. N>1 `gemm_tiled`: RTxNT register tile (4x32), B tile in
  BRAM (partition dim 2), A tile reshaped by 16, accumulators `sh[RT][NT][LACC]` rotating by `t % LACC`.
  Anything with K%16!=0 or K>768 falls back to a slow generic loop (correctness only).
- Knobs (all in `fpga/src/kernel_cfg.h`): RT, NT, LACC, RG, ACC_ROTATE, WIDEN, BURST, OUTSTANDING.
- Known tool facts: `fadd` is the 7-cycle `full_dsp` core and `fmul` is 4 cycles, so partial-sum depth
  must be >= 7 (we use 8). Clock target 3.33 ns with 0.899 ns uncertainty, so the effective logic budget
  is 2.43 ns. v2 csynth: all loops II=1 and 512-bit bursts inferred, but the estimated clock was
  2.47-2.73 ns (over budget) because the shift-register accumulator forced a one-cycle adder. v3 replaces it
  with a rotating index plus `DEPENDENCE ... RAW distance=L dependent=true`; v3 is not yet run through csynth.
- Roofline inputs: A traffic for the four shapes at N=1 totals 38,965,248 B; one 512-bit port at 300 MHz
  is 19.2 GB/s -> ~2.03 ms for the N=1 sum. N=32 sum is ~624 MFLOP and the square case ~906 MFLOP, so
  both are compute-bound by MAC lanes x DSPs (a float MAC is ~5 DSP). Estimates only; confirm with reports.
- csynth estimates are optimistic (the lecture's naive kernel: 33k cycles csynth vs 143k cosim). Rank
  configs by csynth, confirm finalists with cosim and real hardware runs.

## Next steps (HISTORICAL, written Oct 6; the current status and plan are in the last section of this file)
1. User runs v3 csynth on RCE, pastes the loop/latency table and Utilization Estimates (DSP/BRAM/LUT/FF).
2. If timing passes: `make cosim`, then start `./slurm.sh hw` immediately (1 h) while exploring.
3. Design-space exploration: tile size (RT, NT), LACC, A double-buffering (overlap load and compute),
   `ap_uint<512>` ports, A-load/compute dataflow, GEMV row tile, frequency target, multi-bank layouts.
   Build a sweep script that writes `kernel_cfg.h`, runs the local g++ gate, then csynth, and parses reports
   into a CSV; predict kernel ms per scoring case from the roofline model.
4. Submit, read tok/s, iterate. Keep `notes.md` and `BENCHMARKS.md` updated.

## Status at the end of Oct 6 (HISTORICAL; superseded by "Latest status (Oct 8)" at the end of this file)
RCE layout (user's home): `~/fpga` = working dir (v4-equivalent defaults, csynth pending), `~/fpga_v3` =
frozen snapshot of v3 used for the baseline hardware build.
- v3 (ACC_STYLE=1, registers): csim PASS on RCE; csynth OK; all loops II=1; 512-bit bursts inferred; est. clock
  2.729 ns (> the 2.431 ns effective budget; 3.33 ns target minus 0.899 ns uncertainty). Critical path is a
  single `fadd` (2.342 ns) -> store: with register accumulators the tool sees a distance-1 loop-carried scalar and
  forces a one-cycle adder, so the DEPENDENCE pragma does nothing. Utilization: BRAM 199 (7%), DSP 753 (12%),
  FF 136K (7%), LUT 110K (12%). Cores chosen: fadd = 7-cycle full_dsp, fmul = 4-cycle max_dsp.
- v3 cosim: PASS (job 103771, ~10 min). Measured latency: (16,288,N=1) 1081 cyc, (8,768,N=1) ~1098 cyc,
  (4,288,N=40) 52103 cyc (non-aligned B path: 288 short bursts x ~100 cyc latency each; not a graded shape).
  Fit: per-call fixed ~155 cyc, per-8-row-tile overhead ~175 cyc, so v3 GEMV reaches ~26-35% of the 19.2 GB/s
  port. Model predicts ~6.5 ms for the N=1 sum (v3) vs ~2.2 ms with dataflow overlap (floor 2.03 ms).
- v3 hardware build submitted: Slurm job 103773 (`./slurm.sh hw` in ~/fpga_v3), was pending on priority.
  Needs `build/hw/gemm.xclbin`, then `HW_TOKEN=<token> make submit` (token never goes in the repo/chat).
- Kernel source now has knobs in `fpga/src/kernel_cfg.h`: RT, NT, LACC, RG, ACC_STYLE (0 shift regs, 1 rotate in
  regs, 2 rotate in LUTRAM = default, intended fix for the timing path), USE_DATAFLOW (0 default = v4-equivalent,
  1 = v5: load/compute/store DATAFLOW across tiles + wide aligned B-tile load), WIDEN, BURST, OUTSTANDING.
  v5 recommended settings: -DUSE_DATAFLOW=1 -DRT=8 -DRG=16. All variants pass the local g++ gates
  (full tb, cosim-sized tb, 589k-output edge sweep /tmp/edge_hls.cpp equivalent).
- PENDING USER RESULT: v4 csynth (`rm -rf build/hls && ./slurm.sh csynth && cat build/hls/hls/syn/report/gemm_csynth.rpt`).
  Compare timing/area against v3; if the critical path drops below 2.43 ns, ACC_STYLE=2 stays. Then cosim v4,
  then try USE_DATAFLOW=1 (v5) the same way. Only then plan bigger tiles and the memory-hierarchy ideas.
- Design ideas discussed (not implemented): within-call URAM residency of A for large N (safe; helps the
  square case by cutting A re-streaming ~57 MB -> ~2.4 MB); cross-call on-chip weight cache keyed by A address
  (high ceiling for N=1/tok/s but unsafe: stale data if the grader reuses an address, capacity ~20-29 MB of the
  60.8 MB weights, unknown whether the grader keeps the kernel loaded) -> late experiment, flag, default off;
  bigger tile (R=16,NT=32 ~ 512 MACs ~ 3000 DSP), ap_uint<512> ports, HBM bank mapping, higher clock (needs the
  Makefile link step, only if we decide to).
- Cycle model for planning: scoring N=1 sum moves 38,965,248 B of A (port floor ~2.03 ms at 300 MHz); N=32 sum
  ~624 MFLOP; square ~906 MFLOP. v3 estimate ~33-45 GFLOP/s at N=32/square before overlap, 3 GFLOP/s at N=1.

## Goal, framing and targets (decided Oct 6 night)
- Main project = the graded fp32 GEMM kernel on the U50. It is the only route to MEASURED hardware GFLOP/s (via the
  portal); int8 or other kernels could only be argued from csynth/cosim, so they are extensions, not the headline.
- Roofline framing to present: (1) compute-bound cases (square 768^3, N=32): roof = usable MACs x 2 x clock; with the
  tool's default 5-DSP fp32 MAC about 1000 MACs fit -> ~600 GFLOP/s at 300 MHz (fabric adders could ~double the
  MAC budget). (2) N=1 is bandwidth-bound: single 512-bit port = 19.2 GB/s -> ~9.6 GFLOP/s at 0.5 flop/B, so
  report N=1 as % of the bandwidth roof, and take the GFLOP/s headline from the square/N=32 cases.
- Targets (plans, not promises): round 1 = 512-MAC array (peak ~307 GFLOP/s) at >=80-85% utilization = ~250 GFLOP/s
  measured on square, via dataflow overlap + big tile + B tile in URAM; N=1 at >=80-90% of the port; stretch ~1000
  MACs with fabric adders for 400+ GFLOP/s (timing closure is the risk, csynth/P&R tells early).
- Supporting arguments: our measured CPU kernel is 60-100 GFLOP/s on the user's laptop; DSP-roofline percentage;
  predicted-vs-measured cycle agreement (v3 cosim fit already matched); profile-driven story (classifier = 65% of
  matmul time, matmul f~94%); honest-baseline discipline (documented retractions).
- The user will NOT state the deadline (avoids Parkinson's law) and wants maximum work in the available time.
- Tone: be decisive. Do not attach caveats/flags to every message; the user reads repeated hedging as a sign to
  abandon the project. State risks once, briefly, and only when they change a decision.

## Latest status (Oct 8, 2026 ~00:00 IST, end of the Oct 7 session; supersedes everything above where it conflicts)
Read this section, then logs/variants.md, logs/leaderboard_2026-10-07.md and logs/handoff_2026-10-08.md (commands, pitfalls, first action).
- Measured on the card: v3 (portal id 58, kernel clock auto-scaled 300 -> 279.1 MHz): all 9 PASS, square 32.70 ms, N=1 sum 8.80, N=32 sum 24.03, 57.24 tok/s. Leaderboard (14 rows): rank 10 by square, 7th by tok/s. Pre-run model estimates were 1.3-1.7x optimistic; trust measured numbers only.
- v5b (dataflow RT=8) built (4 h, clock auto-scaled to 189.8 MHz) but the portal rejected it: "wrong results: 32000x288x32" (id 76), no scores. Cosim at RT=8 with few tiles had passed; the bug needs thousands of tiles and real memory timing.
- Root cause 1 (correctness, read from RTL): the tile dataflow passed Ct from compute to store as 512 scalar returns wired directly from the fadd cores into depth-2 FIFOs, popped only when the store ends. A store slower than compute fills the FIFOs and the delayed capture takes garbage from the free-running adders. This also explains the RT=12/16 cosim failures (4th/6th tile zero). Fix: Ct partitioned dim=2 only (RAM channel) + row-by-row reduction in the compute epilogue. Verified: RTL has no ap_return and a Ct_RAM PIPO; cosim PASS at RT=16 with 64 and 128 rows (variant r16c).
- Root cause 2 (clock): v5b's 189.8 MHz was one path inside tl_store (a 256-way mux over Ct[r][j], 10 levels of MUXF7/8, 84% routing). The Ct RAM change removes it. The next-worst path is unknown until the new builds report.
- Other findings: predicated store in the A loaders (`if (rr == r)`) keeps II=1 for RT>8 (a dynamic row index drops to II=8); wide 512-bit C store added; RG=32 costs 743 BRAM and made routing take >5 h (use RG<=16); frp unproven; ACC_STYLE=2 dead; slurm.sh hw's 32G OOMs in Vivado synthesis.
- v5bc (RT=8, RG=8, Ct fix) built (job 104027, 3h35) and PASSED on the card: square 11.448 ms, N=1 sum 5.701, N=32 sum 8.678, 76.55 tok/s (rank 8/7/9/7 on a 17-row leaderboard, see logs/leaderboard_2026-10-08.md). Kernel clock still auto-scaled to 187.4 MHz: worst path -2.002 ns is the compute pipeline's stall/enable net (BUFGCE + LUT2, 96% routing), the same fanout path as v3 but bigger. r16c16 (job 104029) was cancelled for the same reason. NEXT BUILDS (user must say yes): v5bcf = v5bc + style=frp on the compute loops; r16c16f = r16c16 + frp. frp passed cosim at RT=8/12. Scaling by clock alone (x1.49 to 280 MHz) puts v5bc at about square 7.7, N=32 5.8, N=1 3.8 ms (tok/s ~106), the rank-5 line. Nothing is running on RCE now. Copy the timing report from a build's _x/reports/link/imp before deleting it.
- Targets (leaderboard): top 3 needs square <= 6.45 ms, N=32 sum <= 4.38 ms, N=1 sum <= 3.21 ms, tok/s ~118; rank 5 square is 12.25 ms. tok/s ~= 1/(3.1 ms + 1.66 x N1sum ms), i.e. decided almost only by the N=1 sum (rank 4 needs <= ~3.76 ms). Per token: 60.8 MB of weights, 30.4 MFLOP, 43 matmuls; one 512-bit port at 280 MHz caps tok/s at ~156 (about 5.7 GFLOP/s of matmul while running at 120 tok/s = 11.5 GB/s = 64% of the port).
- Plan (in order): (1) read the real clock and worst path from the new builds and remove it; (2) RT=16 efficiency (cosim shows ~571 cycles per 16-row tile vs 288 ideal, ~50% busy; find the bottleneck stage); (3) N=1 streaming reader (one long-lived load stage, no per-tile restart) for tok/s; (4) B-tile load (~3000 cycles per call, should be ~600); (5) bigger moves (fabric adders, A resident on chip / URAM). Steps 3 and 4 are independent of the builds.
- Professor pitch: do not say "world class" (llama2.c stories15M runs 85-160 tok/s on desktop CPUs). Argue percent-of-roofline for N=1 and, if a board power reading is possible (xbutil), tok/s per watt versus a CPU. The user cares about leaderboard rank (wants top 3, top 1 is 777 GFLOP/s on square) and about energy efficiency.

## Latest status (Oct 8, 2026 ~19:15 IST evening; supersedes the section above where it conflicts)
Read this section, then logs/handoff_2026-10-08_evening.md (full state, commands, rules, first action), logs/variants.md, and the last section of ideas2.md. DO NOT build or create variants until the user says so.
- Card results (all 9/9 PASS): v5bcf (id 100) 187.4 MHz square 11.45 / N=1 5.71 / N=32 8.67 ms / 77.0 tok/s; e2 (id 113) 283.5 MHz 7.77 / 6.72 / 5.80 / 67.0; e2b (id 114, + B-port outstanding fix) 288.5 MHz 7.27 / 6.68 / 5.72 / 70.0; e2c (id 116, + tree epilogue) 279.1 MHz 7.27 / 6.71 / 5.59 / 64.9. Square and N=32 improved ~35% from the clock fix; N=1 got slightly worse because the streaming gemv (cosim PASS) is slower on the card.
- Clock root cause (measured): one accumulator reset net (-2.002 ns, 65k pins) set 187.4 MHz; removed with ZINIT=1 + CP compute split + epilogue II fix; remaining limiter is the valid-bit enable net into the accumulators (~1.1k loads per process). Clock varies +-3% between near-identical builds.
- Resources: FP32 MAC = 5 DSP, ~625 LUT; csynth gate before any hardware build (DSP<95%, LUT<80%, BRAM<85% of one SLR). RT=16 fails the LUT gate; RT=12 (CP=6) is the next size up (est. 2.05K DSP, ~311K LUT, BRAM 86% with the streaming gemv).
- Cycle model (tile = K + 13 + epilogue; epilogue 63 -> 29 cycles with the tree; B-tile load 2920 -> 1072 cycles per call) is calibrated on the card to 1-4% for N=32/square.
- N=1 is the open problem: A reads only reach ~5.8-6.9 GB/s (top rows ~12.6 GB/s). All of A, B, C sit on HBM[0] (single pseudo-channel); the L4 handout says banks are a link-time `sp=` choice. Running now: N=1 lab on RCE (RT=4 CP=2 designs; the first RT=2 CP=1 attempt blew up csynth and was cancelled, CP=1 must never be used): n1a 104483 (streaming control), n1b 104484 (A outstanding 64), n1c 104485 (A burst 128), n1d 104486 (old gemv), n1e 104487 (separate HBM banks via connectivity.cfg + link_sp.sh). Launched ~19:20 IST, expected done ~20:45-21:15; submit each xclbin and compare N=1 times.
- Plan after the lab: pick the N=1 setup, then two full builds gated by csynth: e5 = RT=8 CP=4, e4 = RT=12 CP=6 (needs per-index CP call guards in gemm_tiled, see the handoff).
- RCE quirks: always `--exclude=node05` (missing libtinfo.so.5); run scripts with `bash ./scripts/...`; short time limits and small memory start sooner in the FIFO queue.

## Next steps (continuing past the 9 Oct deadline) (written 2026-10-09; details in WRITEUP.md section 11 and ideas2.md)
Choose a base architecture from the builds in flight (k4a/k4s/k2d, b4a/b6/b8 with SSI_BalanceSLRs; rule: most MACs that routes at about 270 MHz or better). Then: (1) on-chip fp16 weight cache under the fp32 contract (int8 and bf16 fail the 1e-2 gate, measured), (2) own-host int8 whole-model-on-chip for stories15M and multi-bank HBM streaming for bigger models, (3) power and CPU/GPU baselines for a workshop-style claim. Open question for the professor: whether the older host-bundle submission flow still works (needed to measure 1-2 on the card). Never run RCE steps that delete a build folder before harvesting; csynth-gate every new array size.
