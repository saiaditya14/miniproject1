# Mini-Project 1 (roll 2023101096): GEMM on CPU and on the U50, and the language model that runs on it

Start with `docs/WRITEUP.md`. It is the report; section 1a maps every checkpoint of the three course pages to its answer and says what was measured, what is modelled, and what cannot be measured and why. Section 12 maps every claim to its evidence.

## Results in one table (card numbers from the grader portal)

| | clock | square 768^3 | N=1 total (4 shapes) | N=32 total (4 shapes) | TinyStories tok/s |
|---|---|---|---|---|---|
| selected design k2d (384 MACs), leaderboard run 191 | 262.3 MHz | 6.436 ms | 3.227 ms | 5.281 ms | 107.78 |
| k2d, median of eight runs (184-191) | 262.3 MHz | 6.456 ms | 3.244 ms | 5.281 ms | 106.1 |
| baseline f4 (256 MACs), best tok/s run 178 | 281.2 MHz | 7.141 ms | 3.242 ms | 5.400 ms | 111.31 |
| f4, median of nine runs | 281.2 MHz | 7.149 ms | 3.246 ms | 5.406 ms | 105.4 |
| leaderboard rank at 22:26 on 9 Oct (run 191 live) | | 8th | 4th | 8th | 5th |

## Where each deliverable is

| what | where |
|---|---|
| Part 1 float32 and int8 GEMM (CPU) | `part1_cpu/student_gemm.c`, `part1_cpu/student_gemm_int8.c`; results and method: `docs/BENCHMARKS.md`, WRITEUP 1b; earlier hand-written versions: `part1_cpu/loop.c`; OpenBLAS comparison: `part1_cpu/bench_openblas.c` |
| Part 2 profile, batching, offload ranking (CPU) | `part2_batching/`, `docs/notes.md`, WRITEUP 5 |
| Part 2 on the accelerator (U50) | source of the selected bitstream: `fpga/k2d/` (`src/kernel.cpp`, knobs in `src/kernel_cfg.h`: RT=6, CP=2, KUN=2); the 256-MAC baseline it was developed from: `fpga/f4/`; results, story and tok/s: WRITEUP 1; per-token budget and ceiling: WRITEUP 4 |
| Part 3 int8 seam and precision sweep | `part3_int8/`, WRITEUP 9, figures in `results/figures/` |
| every other FPGA variant (flags, job ids, outcomes, retractions) | `fpga/variants.zip` (one folder per variant), `results/logs/variants.md` |
| fix in progress at the deadline (streaming A reader for the tiled path) | `fpga/sk4/`; its RTL cosim passed and its hardware build was still running when this was packed, so it has no measured result (WRITEUP 1, "Known limitations") |
| build logs, timing reports, portal rows | `results/evidence/`, `results/logs/` |
| engineering narrative behind the results (clock closure, debugging log, the K-unrolled builds, next steps), kept out of the report | `docs/ENGINEERING_LOG.md` (sections 6, 7, 8, 10, 11 in full; the report carries a short summary of each) |
| how to rebuild and resubmit | `docs/REPRODUCE.md` |

No portal token is stored anywhere in this archive.
