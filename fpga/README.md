# FPGA kernels (Alveo U50, Vitis HLS)

A single `extern "C" void gemm(const float *A, const float *B, float *C, int M, int K, int N)` kernel, fp32, one bitstream for every shape.

| folder | what it is | clock | square 768^3 / N=1 total / N=32 total / tok/s (portal, measured) |
|---|---|---|---|
| `k2d/` | the selected design: 384 MACs (6 rows x 32 columns x 2 k per cycle, two compute processes) | 262.3 MHz | 6.436 ms / 3.227 ms / 5.281 ms / 107.78 (run 191; median of eight runs 6.456 / 3.244 / 5.281 / 106.1) |
| `f4/` | the 256-MAC baseline k2d was developed from | 281.2 MHz | 7.141 ms / 3.242 ms / 5.400 ms / 111.31 (run 178, the best of nine runs; median of nine 7.149 / 3.246 / 5.406 / 105.4) |
| `sk4/` | k4a (512 MACs) plus a persistent streaming reader for A on the tiled path; cosim passes; the hardware build closed at 123.4 MHz | 123.4 MHz | 10.316 ms / 5.653 ms / 9.670 ms / 68.71 (run 196, after the leaderboard deadline); about 30% fewer cycles on square and 19% fewer on N=32 than k4a |
| `variants.zip` | the other variants built before the final ones, one folder each | | see `../results/logs/variants.md` |

The three folders differ only in `src/kernel.cpp` and `src/kernel_cfg.h` (knobs `RT`, `NT`, `CP`, `KUN`, `TL_STREAM`, ...). Build and submit commands: `../docs/REPRODUCE.md`. The report: `../docs/WRITEUP.md`, with the engineering narrative in `../docs/ENGINEERING_LOG.md`.
