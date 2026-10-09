# Handoff for the next session (written 2026-10-08 ~19:15 IST). Supersedes handoff_2026-10-08.md.

Read, in this order: `CLAUDE.md` (last section "Latest status (Oct 8 evening)"), this file, `logs/variants.md` (every variant, job id, measurement), `ideas2.md` (last section "KEY OBSERVATIONS AND PROPOSED NEXT STEP"). The memory index lists the working-style rules; follow them.

## FIRST ACTION
Do NOT build, create folders, or launch jobs on your own. The user explicitly said (twice) not to. First read the files above, then ask for the lab results (command below) and wait for the user's go-ahead before creating anything.

## Where things stand (hardware results, all 9/9 PASS unless noted)
| portal id | design | kernel clock | square ms | N=1 sum ms | N=32 sum ms | tok/s |
|---|---|---|---|---|---|---|
| 58 | v3 (RT=4) | 279.1 | 32.70 | 8.80 | 24.03 | 57.2 |
| 76 | v5b | 189.8 | FAILED wrong results 32000x288x32 | | | |
| 84 | v5bc (RT=8) | 187.4 | 11.448 | 5.701 | 8.678 | 76.55 |
| 100 | v5bcf (= v5bc + frp) | 187.4 | 11.450 | 5.714 | 8.667 | 77.02 |
| 113 | e2 | 283.5 | 7.767 | 6.724 | 5.802 | 67.0 |
| 114 | e2b (= e2 + B-port fix) | 288.5 (hbm_aclk scaled 450->441.6) | 7.269 | 6.680 | 5.722 | 69.96 |
| 116 | e2c (= e2b + tree epilogue) | 279.1 | 7.268 | 6.712 | 5.588 | 64.89 |
Per-case tables: logs/runidk.md (ids 100 and 58). The leaderboard (per the user) shows the best tok/s submission per person. Rank lines seen at ~01:00 IST: square rank 5 = 7.463; N=32 rank 5 = 5.196, rank 7 = 5.629; N=1 rank 5 = 3.242, rank 6-7 = 3.75; tok/s top = 118.7. We are about rank 5 on square, ~7 on N=32, ~9-10 on N=1 and tok/s. Re-check the board, it moves.

## Key findings (all measured, evidence in logs/ and ideas2.md)
1. Clock history. v5bc/v5bcf: worst path -2.002 ns = one accumulator RESET net (ap_loop_init_int -> sh reg R pins, ~65k pins through a BUFGCE, 96% routing); 3.333+2.002 = 5.335 ns = 187.4 MHz. frp did NOT help. Fix that worked: no accumulator reset (ZINIT=1: first LACC iterations use 0 instead of the stale partial sum) + compute split into CP processes + epilogue II fix. e2 routed WNS -0.194 ns -> 283.5 MHz. Worst paths now = valid-bit enable net into accumulators inside tl_compute (ap_enable_reg_pp0_iter11 / icmp_ln411_reg_..._pp0_iter9/10 -> empty_*/p_load* regs), 3-4 LUT levels, ~90% routing, ~1.1k loads per process at RTP=4. Clock varies about +-3% between near-identical builds (283.5, 288.5, 279.1).
2. Resources. FP32 MAC = 5 DSP (3 fmul + 2 fadd), ~625 LUT per MAC. The first split (e.g. v8qz, m1pz) duplicated the epilogue adders per process (epilogue at II=1 needs 224 adders/cycle > MAC adders) -> 2947 DSP (99% of an SLR) and 3717 DSP (124%). Fix: epilogue II = ceil(7/RTP) (macro EPII) so it shares the MAC adders (epilogue DSP 386 -> 2). csynth gate before any hardware build: DSP < 95%, LUT < 80%, BRAM < 85% of ONE SLR (SLR = 2976 DSP, 435840 LUT, 1344 BRAM18). e2 (RT=8, CP=2): 1413 DSP / 224K LUT / 814 BRAM18 = 47/51/60%. e3 (RT=16, CP=4): 2693 / 399K / 1070 = 90/91/79% -> FAILS the LUT rule, do not build RT=16.
3. Cycle model, calibrated on the card to within 1-4% for the tiled path: tile = K + 13 (main loop depth) + epilogue. Old chain epilogue 63 cycles -> tree epilogue 29: 364 -> 335 cycles per 8-row tile at K=288 (cosim, e2c). B-tile load: 2920 -> 1072 cycles per K=288 call with B port max_read_burst_length=8, num_read_outstanding=64 (cause: B reads were not burst-inferred, 16 outstanding x ~80 cycle latency = 5 cycles/word). e2 hardware: 32000x288 N=32 = 383 cycles/tile (model 379); square 2.20M cycles (model 2.13M). Square/N=32 scale with MACs x clock. 32000x288 is ~94% of the N=1 sum and ~93% of the N=32 sum.
4. N=1 is the open problem. The streaming gemv (GV_MODE=2, cosim PASS) is SLOWER on the card than the old per-tile gemv: e2 32000x288 N=1 = 6.347 ms = 1.80M cycles = 3.12 cycles/word = 5.8 GB/s of A (11.0 ns/word); v5bcf old gemv 5.356 ms = 251 cycles per 8-row tile = 6.9 GB/s (9.3 ns/word) at 187 MHz. Both are ~6-7 GB/s in time, not cycles. Top leaderboard rows read ~12.6 GB/s (N=1 sum ~3.1 ms, ~118 tok/s) from the same single channel. e2 RTL shows the A request is one ARLEN=trip-count burst split into 64-beat bursts, 16 outstanding, 1024-beat buffer, i.e. nothing wrong in the adapter. The square case sustains ~7.8 GB/s of A reads on the same port, so the channel can deliver more than 6 GB/s. Cause still unknown.
5. HBM placement. gemm.xclbin.info shows A, B and C all on HBM[0] (one 256 MB pseudo-channel, peak 14.4 GB/s). The course Makefile link step has no connectivity config. The L4 handout (lecture/gemm_fpga_lect.pdf page 8) says: bundle= makes separate AXI masters but the HBM bank is an explicit link-time choice (`sp=gemm_1.B:HBM[1]` in a connectivity.cfg); distinct banks give independent bandwidth, same bank contend. The user recalls the prof also covered this in the early lectures (l1-l3 pdfs in ../midsem); not read yet. The starter has no host code, so it is unknown whether the grader's host honors xclbin banks.
6. Cosim notes: the `M=.. PASS` lines in a cosim log come from the C run; the real gate is `C/RTL co-simulation finished: PASS`. Cosim is slow (xsim single threaded); the N=40 case from the starter tb dominates (175 us sim). The cosim memory model hides HBM effects (streaming gemv passed cosim and lost on the card).
7. Tooling facts: `scripts/*.sh` lose the exec bit on SFTP, run them with `bash ./scripts/...`. Node05 is missing libtinfo.so.5, Vitis HLS dies in seconds there: use `--exclude=node05` on every sbatch. The queue is FIFO priority 1; big memory requests (64G) wait; short time limits and small memory backfill. The leaderboard URL `http://10.4.25.39:18080/leaderboard`; the user's personal results page contains the token, never print it or ask for it back.

## Running on RCE right now (N=1 lab, RT=4 CP=2 ZINIT=1, 128 MACs, folders rce_upload/fpga_n1a..n1e)
IMPORTANT history: the first lab (RT=2, CP=1; jobs 104447-104473) was cancelled. With CP=1 csynth exploded at the 'Array/Struct' phase (4,609 -> 390,759 instructions, stale log) because the size-1 leading dimension of Bbuf/At/Ct breaks the partition pragmas (probable). CP=1 was never csynth-gated; ALWAYS csynth-gate a new RT/CP combination (CP >= 2 only). The lab was re-cut at RT=4 CP=2: csynth gate on n1a = 108 s, 7-9k instructions, DSP 769 / LUT 157K / BRAM18 743 (25/35/55% of one SLR).
| job | variant | change vs n1a |
|---|---|---|
| 104483 | n1a | control: streaming gemv (GV_MODE=2 RG=64), A port burst 64 / outstanding 16 |
| 104484 | n1b | A port OUTSTANDING=64 |
| 104485 | n1c | A port BURST=128, OUTSTANDING=32 |
| 104486 | n1d | old gemv (GV_MODE=0, RG=8) |
| 104487 | n1e | n1a + connectivity.cfg (A:HBM[0], B:HBM[1], C:HBM[2]) linked by link_sp.sh |
Status snapshot 19:26 IST: n1a (node06) 7 min in, block-level synthesis 7/117; n1b and n1c (node03) block-level synthesis 0/117; n1d (node04) already at the link step; n1e still pending (Slurm's estimate: start ~19:36). The cluster is MEMORY-bound (other students reserve 96G/64G jobs), not CPU-bound: only nodes with >=24 GB unreserved can take our jobs; start times come from other jobs ending. Expected finish: n1a/b/c ~20:30-21:00, n1d a few minutes later, n1e ~21:00-21:30.
Submitted ~19:20 IST with --exclude=node05 --cpus-per-task=6 --mem=24G --time=03:00:00; all five were pending (Priority) at launch. Expected 1-1.5 h after start, i.e. roughly 20:45-21:15 IST. Purpose: find why A reads get ~6 GB/s. Only the N=1 column is meaningful (N=32/square are slow by design but must PASS). When each xclbin appears: `cp` it to `~/keep/<name>.xclbin`, submit, read n1_ms (and per-case n1 times with `hw-status.sh <id>`). n1e failing with an error/wrong results most likely means the grader host ignores xclbin banks.
Lab results command:
```
date; squeue -u $USER -o "%.8i %.14j %.3t %.10M %.8R"; for n in n1a n1b n1c n1d n1e; do echo "$n: $(tail -n 1 $(ls -t ~/rce_upload/fpga_$n/build/logs/hw-*.log 2>/dev/null | head -1) 2>/dev/null | cut -c1-100) | xclbin: $(ls ~/rce_upload/fpga_$n/build/hw/gemm.xclbin 2>/dev/null)"; done; quota -s | tail -1
```

## Plan after the lab (decided with the user; nothing is built yet)
1. Read all five N=1 numbers together; pick the N=1 setup (gemv mode, A-port parameters, HBM banks). If more than one knob helped, stack them. If none helped, read the RTL of the read path more and look at the small-call fixed costs.
2. Two full builds, one change set each, gate each with csynth only first (about 8-10 min): 
   - e5 = RT=8, CP=4 (RTP=2, steadier clock, halves the enable net), best N=1 setup. cfg-only on e2c (CP is already a cfg knob).
   - e4 = RT=12, CP=6 (RTP=2): needs a small code change in gemm_tiled: per-index guards `#if CP > i` for tl_compute(At[i], Bbuf[i], Ct[i]) for i=1..7 (the current code groups calls [2],[3] and [4..7]) and the static_assert allowing CP in 1..8 with RT % CP == 0 (currently {1,2,4,8}). Estimates (scaled from e2/e3): ~2.05K DSP (69%), ~311K LUT (71%), BRAM ~1156 (86%) with the streaming gemv (512 BRAM18 for gv) or ~780 with the old gemv. If BRAM > 85% drop the streaming gemv or use RG=32. Model: N=32 ~3.6 ms, square ~4.6 ms at ~285 MHz (rank ~3-4). RT=16 does not fit.
3. Always: one change per build, csynth gate, at most 2-3 builds in flight, harvest before deleting anything tied to an open issue.
4. Later options: B load 1072 -> ~600 cycles/call, 13-cycle loop depth, A resident in URAM for square, the cross-call weight cache (idea 4 in ideas2.md, risky, last).
Odds I gave (judgment, not measurements): RT=12 clock 240-290 MHz ~70%, below 200 ~10%.

## DECISION (made with the user, 2026-10-08 evening): build e5 = RT=8 CP=4 and e4 = RT=12 CP=6, each csynth-gated first, with the gemv that wins the N=1 lab (most likely the old gemv, GV_MODE=0). If the e4 csynth gate shows BRAM > 85% of one SLR, use the old gemv (about 780 BRAM18); do not re-debate the CP values. The notes below are caveats only.
## Confidence in the e5 / e4 parameters (caveats, not a reason to hesitate)
These are PROPOSALS from my analysis, not verified settings. Rule: CP must divide RT and CP >= 2 (CP=1 breaks csynth); RTP = RT/CP rows per process.
- Measured: the clock limiter at RTP=4 (e2 family) is the valid-bit enable net into the accumulators, ~1,150 loads per process (routed timing report logs/e2_timing_routed.rpt; RTL control-net count). v8qz's RTL (RTP=2, old epilogue) had ~580 loads on that net.
- Partly verified: RTP=2 is cheap in DSP/LUT with the epilogue II fix: lab n1a (RT=4 CP=2) csynth = 769 DSP / 157K LUT / 743 BRAM18, 108 s, 7-9k instructions. No cosim or hardware yet for RTP=2 with the fix.
- Inferred only: that RTP=2 (e5 = RT=8 CP=4, e4 = RT=12 CP=6) moves the clock toward 300 MHz. This is exactly what e5 tests. Build noise is +-3%, so a single build proves little unless the gain is larger than that.
- Estimated only (scaled from e2/e3, never run through csynth): e4 = ~2.05K DSP, ~311K LUT, BRAM ~1156 (86%, over the 85% gate) with the streaming gemv or ~780 with the old gemv. CP=6 also needs 6 B-tile copies (~64 BRAM18 each). Lower-risk alternative if BRAM fails: RT=12 CP=3 (RTP=4, e2-size processes, 3 B copies, ~960 BRAM with streaming gemv, expect an e2-like clock).
- Not written yet: the CP=6 code (per-index guards `#if CP > i` in gemm_tiled and the static_assert allowing CP in 2..8 with RT % CP == 0). Local gate (tb, full, N=1 and N-sweep edge tests) must pass for RT=12 CP=6 including M not a multiple of 12 (32000 = 2666*12 + 8).
- Order: csynth gate first (DSP<95%, LUT<80%, BRAM<85% of one SLR, instruction count sane at the 'Array/Struct' phase), then hardware. The user decides which builds to start.
- Odds I gave the user (judgment): RT=12 clock 240-290 MHz ~70%, below 200 MHz ~10%.

## Command library (RCE, user's home; the user pastes outputs back)
Hardware build (always exclude node05; small designs use less):
```
cd ~/rce_upload/fpga_<name> && rm -rf build/hw && mkdir -p build/logs && sbatch --exclude=node05 --job-name=gemm-hw-<name> --partition=debug --nodes=1 --ntasks=1 --cpus-per-task=8 --mem=64G --time=06:00:00 --output="build/logs/hw-%j.log" --wrap="make hw"
```
(Full-size RT=8+ designs used --mem=96G --time=10:00:00; a shorter limit and less memory start sooner. Builds take ~3.5-4.5 h for 256 MACs, ~1-1.5 h for the 64-MAC lab.)
csynth-only gate: same with `--time=01:00:00 --mem=32G --output="$HOME/<name>_csyn.log" --wrap="make csynth"`; then read `build/hls/hls/syn/report/gemm_csynth.rpt` Total / Utilization SLR rows and the tl_compute*_csynth.rpt rows (epilogue module `Pipeline_*_466_12`-style should show ~2 DSP).
Status of a build: `tail -n 1 $(ls -t ~/rce_upload/fpga_<n>/build/logs/hw-*.log | head -1)`; final clock: `grep -h AUTO-FREQ ~/rce_upload/fpga_<n>/build/logs/hw-*.log | cut -c1-250` (no line = 300 MHz met); routed timing: `grep -A6 "Design Timing Summary" .../prj.runs/impl_1/hw_bb_locked_timing_summary_routed.rpt | tail -3` (clock = 1000/(3.333 - WNS)).
Harvest (before deleting): copy gemm.xclbin.info, the hw log, the routed timing report (`build/hw/_x/link/vivado/vpl/prj/prj.runs/impl_1/hw_bb_locked_timing_summary_routed.rpt`), the xclbin to ~/keep, zip, SFTP into logs/.
Submit and read: `export HW_TOKEN=$(cat ~/token.txt | tr -d ' \n\r'); cd ~/rce_upload/fpga_<n> && bash ./scripts/hw-submit.sh build/hw/gemm.xclbin 2>&1 | grep -v "token=" | cut -c1-200; sleep 20; bash ./scripts/hw-status.sh | python3 -m json.tool | head -17`. Per-case: `bash ./scripts/hw-status.sh <id> | python3 -c "import sys,json; print(json.load(sys.stdin)['log'])" | grep -E "^  (n1|n32|square)"`.
Quota 20 GB (`quota -s | tail -1`). Xclbins kept in ~/keep: v3, v5bc, e2, e2b (e2c: verify `ls ~/keep`). Finished builds can be deleted after harvest; folders with running jobs must not be touched.
Local gate for every variant (no Vitis; needs a tiny hls_stream.h shim in a folder passed with -I, only for GV_MODE=2): `g++ -O2 -std=c++17 -Wno-unknown-pragmas -I <shim> -o tb <folder>/src/kernel_tb.cpp <folder>/src/kernel.cpp && ./tb && ./tb full`, plus N=1 and N-sweep edge tests (the scratchpad files edge.cpp/edgen.cpp from this session are volatile; rewrite them: random A,B, M in {1,3,7,8,9,13,16,63,64,65,72,100,127..129,136,200,288,1000}, K in {16,32,48,128,288,768}, N=1; and N in {2,8,16,31,32,33,48,64,96,100,128,160}).

## Local folders (rce_upload/, source only): e2, e2b, e2c (the three submitted designs), e3 (RT=16, fails LUT gate), n1a..n1e (lab), and older variants. kernel_cfg.h knobs: RT, NT, CP (compute processes), LACC, RG (gemv tile rows), GV_MODE (0 old per-tile gemv, 1 flat At direct, 2 streaming reader), GV_FIFO, ZINIT, ACC_STYLE, USE_DATAFLOW, WIDEN, BURST, OUTSTANDING (A port and C writes), BBURST/BOUT (B port, e2b lineage), EPII is derived from RTP.

## Rules and user preferences learned (follow these)
- Never `git commit`/push. No comments in code files. Docs (md) are fine. Never print or store the HW_TOKEN or the results URL with `token=`.
- Do not create variants/folders, start builds or write handoffs without being asked. The user got angry several times at unrequested actions, hedging, long probe chains, and at speculation. Be decisive, state a risk once, give numbers with units; mark estimates as estimates (several of my estimates were wrong today: frp, the streaming gemv, and the split's resource cost).
- Name job ids and designs before any `scancel`. Hardware only after cosim passes unless the user waives it (they waived it for e2/e2b/e2c/lab to save time).
- Download before delete only when an issue is open; deleting the v5bcf impl folder lost a routed checkpoint once and the user was angry.
- The user credits today's clock win to insisting that I read the reports/RTL; when in doubt, read the data (reports, RTL, per-case tables, the course handouts) before saying there is nothing more to do.
- The user wants top-of-leaderboard results and a roofline story for the professor (do not say "world class"; argue percent of roofline: square 125 GFLOP/s = ~86% of this array's peak at the achieved clock).
- Not true any more: "nothing in the course-provided files has been modified". The variant folders' kernel_tb.cpp (cosim cases, COSIM_*_ELEMS) and the m_axi `depth=` values were edited for local cosim; the submitted artifact is only the xclbin and `depth` only affects cosim.
