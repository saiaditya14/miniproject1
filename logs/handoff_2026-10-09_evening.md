# Handoff (written 2026-10-09 ~16:30 IST). Supersedes handoff_2026-10-08_evening.md.

Read in this order: `CLAUDE.md` (working rules; its last two sections), this file, `WRITEUP.md` (the report, sections 1-12), `logs/variants.md` (last sections), `ideas2.md` (last section). The memory index lists the user's style rules; follow them.

## Where things stand
- Deadline: 9 October (tentatively 11:00; read as tonight). The work continues as the user's main project afterwards.
- Submitted and starred on the portal: **run 149** (f4 bitstream, 4 runs 146-149): 106.61 tok/s, square 7.166, N=1 3.237, N=32 5.393 ms. Board (25 rows, 10:48 snapshot): N=1 4th, tok/s 5th, square 6th, N=32 9th. Medians of the four runs: 7.160 / 3.233 / 5.404 ms / 105.5 tok/s. f1 (run 138) is the same design without the post-route phys-opt pass.
- Builds in flight (all HOME mode, never /tmp): `104893 gemm-hw-k4a` (RT=4 CP=2 KUN=4, 512 MACs; synthesis done 15:29, in implementation; est. xclbin ~19:30-21:00; ~40% to route), `105026 gemm-hw-k2d` (RT=6 CP=2 KUN=2, 384 MACs, started ~15:55 on node03; est. ~21:00-22:30; the safe one, ~55% to route), `105027 gemm-hw-b4a` (k4a design + SSI_BalanceSLRs, pending). Not queued, waiting for disk quota: k4s (k4a + SSI_SpreadLogic_high), b6 (768 MACs, BalanceSLRs, 96G 12h), b8 (1024 MACs). Folders: `rce_upload/fpga_<name>` locally and `~/rce_upload/fpga_<name>` on RCE; archive `rce_upload_variants.zip` is what git tracks (`rce_upload/` is ignored).
- If an xclbin appears: copy to `~/keep/<name>.xclbin`, harvest reports, submit it 3 times (45 s apart), compute medians, star the best run on the portal ("use this run"), and update WRITEUP.md section 1 and 10 (the in-progress table).
- Part 1 (CPU GEMM) and Part 3 (int8 seam and precision sweep, `part3/`) are done. WRITEUP.md is a complete report with placeholders only for pending build results. Evidence archives are in `evidence/`; figures in `figures/`.
- An email to the professor (about the quantisation/main-project direction) was drafted with the user; they were about to send it. Contents: current results stated plainly, ideas (stitching across both SLRs, on-chip weights, multi-bank HBM streaming, tok/s per watt), and the question of what would make it worthy of a main project.

## Key technical findings (details in WRITEUP.md)
- N=1 was limited by single-beat C writes (the tiled path's 512-bit store widened the shared gmem2 bundle), not by reads. Probe kernels la/le/lb/pa read A at 12.5 GB/s = 86-87% of one HBM pseudo-channel (14.4 GB/s). Banks and outstanding count do not matter.
- tok/s per token = ~3.2 ms host + 4.86 ms streaming 60.8 MB + ~1.6 ms per-call cost; harness floor ~208 tok/s.
- Clock limiter is the pipeline-valid/enable net into accumulators (f4 report == f1's). frp was never applied by HLS; the stall signal is constant 0.
- RT=10 and RT=12 builds failed to route ("global congestion level 7"): the placer kept the kernel in SLR1 (70-81% LUT) with SLR0 84% empty. A kernel CAN span SLRs (UG1700); `SSI_BalanceSLRs` is the lever (untested on hardware). LUT fit: LUT ~ 86K + 28K x compute-processes + 0.29K x MACs (5 points).
- fp16 weights pass the 1e-2 gate (0.003-0.008), int8 (0.075-0.154) and bf16 fail: on-chip weight cache idea (WRITEUP section 11, estimate 165-185 tok/s, floor 208).

## Hard-won rules (follow these)
- **Never build in /tmp**: the platform's encrypted IP fails ("Error generated from encrypted envelope"). Build only from the home folder. Manage the 20 GB quota (`quota -s | tail -1`) by limiting concurrent builds (~4-5 GB each; a 768-MAC build ~8 GB). Check quota with every status.
- Always csynth-gate a new RT/CP/KUN size before a hardware build (skipping it cost two 6 h builds). Gate: DSP and BRAM and LUT per SLR; main loops must be II=1 (epilogue II=4 and generic fallback II=8 are expected).
- Read the failure log BEFORE `rm -rf build` (it destroyed k2d's evidence once). Harvest reports before deleting a build folder tied to an open issue.
- Scheduler start estimates are hours too pessimistic; do not cancel and resubmit pending jobs (a resubmission goes to the back); `--test-only` dates of 2042 are placeholders.
- Always give portal commands with `export HW_TOKEN=$(cat ~/token.txt | tr -d ' \n\r');` in the same command; never print the token or a URL containing `token=`. Run scripts with `bash ./scripts/...`. Always `--exclude=node05`.
- The user: decisive answers, no hedging, no "can't promise a grade" disclaimers, no unrequested actions on RCE beyond what they ask, name job ids before any scancel. Never `git commit`. No comments in code. All four leaderboard columns matter equally (the board can be sorted by any). The course allows AI use; the user already told the professor.

## First action in the new session
Ask for the status one-liner (phase of k4a and k2d, quota, queue):
```
for n in k4a k2d; do l=$(ls -t ~/rce_upload/fpga_$n/build/logs/hw-*.log | head -1); echo "== $n | xclbin: $([ -f ~/rce_upload/fpga_$n/build/hw/gemm.xclbin ] && echo YES || echo no)"; grep -E '^\[[0-9:]+\] (Phase [0-9]+ |Run vpl: Step)' $l | tail -3 | cut -c1-90; echo "errors: $(grep -c 'ERROR:' $l)"; done; date; quota -s | tail -1; squeue -u $USER -o "%.8i %.12j %.3t %.10M %.8R"
```
Routing verdict for k4a is expected ~17:30-18:30: a `Phase 3 Global Routing` line means it passed; `Run vpl: Step impl: Failed` with "congestion level" means it did not.
