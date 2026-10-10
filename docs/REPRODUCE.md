# Reproducing the builds and measurements

Hardware builds run on the RCE cluster (never on the login node). Every variant has its own folder (`fpga/variants/`, one folder per variant, flags and job ids in `results/logs/variants.md`); the design behind the selected run 191 is `fpga/k2d/` (the 256-MAC baseline is `fpga/f4/`).

## Local correctness gate (no Vitis needed)

```
g++ -O2 -std=c++17 -Wno-unknown-pragmas -o tb <folder>/src/kernel_tb.cpp <folder>/src/kernel.cpp && ./tb && ./tb full
```

Variants that use `hls::stream` (the streaming N=1 reader, and `fpga/sk4/`) need a small `hls_stream.h` shim on the include path (`-I <dir>`), for example a class wrapping `std::queue` with `write()` and `read()`. Edge-shape sweeps used random A and B, M in {1, 3, 4, 7, 8, 9, 13, 16, 63..65, 72, 100, 127..129, 136, 200, 288, 1000}, K in {32, 48, 128, 288, 768}, N in {1, 2, 8, 16, 31, 32, 33, 48, 64, 96, 100, 128, 160}, checked against a double-precision reference at 1e-2.

## HLS and hardware build on RCE

```
cd ~/rce_upload/fpga_<name> && mkdir -p build/logs
# C synthesis (gate: main loops II=1, DSP/LUT/BRAM per SLR, 0 errors)
sbatch --exclude=node05 --job-name=gemm-csynth-<name> --partition=debug --nodes=1 --ntasks=1 --cpus-per-task=8 --mem=32G --time=01:00:00 --output="build/logs/csynth-%j.log" --wrap="make csynth"
# RTL co-simulation
sbatch --exclude=node05 --job-name=gemm-cosim-<name> --partition=debug --nodes=1 --ntasks=1 --cpus-per-task=8 --mem=32G --time=03:00:00 --output="build/logs/cosim-%j.log" --wrap="make cosim"
# hardware bitstream (about 4-6 h), build/hw/gemm.xclbin
sbatch --exclude=node05 --job-name=gemm-hw-<name> --partition=debug --nodes=1 --ntasks=1 --cpus-per-task=8 --mem=64G --time=10:00:00 --output="build/logs/hw-%j.log" --wrap="make hw"
```

Build only from the home folder: building under node-local `/tmp` fails with an encrypted-IP error (section 7 of the report). Home quota is 20 GB.

Final kernel clock: `grep -h AUTO-FREQ build/logs/hw-*.log` (no line means the 300 MHz target was met; otherwise the clock is 1000 / (3.333 - routed WNS)). Routed timing report: `build/hw/_x/reports/link/imp/impl_1_hw_bb_locked_timing_summary_routed.rpt`.

## Submission and results

The portal token is read from a file and never printed or stored in the repository:

```
export HW_TOKEN=$(cat ~/token.txt | tr -d ' \n\r'); cd ~/rce_upload/fpga_<name> && bash ./scripts/hw-submit.sh build/hw/gemm.xclbin
export HW_TOKEN=$(cat ~/token.txt | tr -d ' \n\r'); bash ./scripts/hw-status.sh          # all submissions
export HW_TOKEN=$(cat ~/token.txt | tr -d ' \n\r'); bash ./scripts/hw-status.sh <id>    # one run: per-case rows, story, log
```

The leaderboard row is the run starred with "use this run" on the personal results page. Repeat submissions of the same bitstream are cheap; the report quotes medians.

## Part 1, 2 and 3 on the CPU

```
gcc -O3 -o test part1_cpu/student_gemm.c ... -lm      # as the course scripts do (GEMM_SRC=part1_cpu/student_gemm.c ./scripts/test_gemm.sh)
gcc -O3 part1_cpu/bench_openblas.c part1_cpu/student_gemm.c -lopenblas -lm   # OMP_NUM_THREADS=1; OpenBLAS comparison
```

Part 2 batched forward and offload ranking: `part2_batching/`. Part 3 int8 seam and precision sweep: `part3_int8/` (`build_runq_slm.sh`, `int8_backend.c`, `sweep.py`, results in `part3_int8/results/sweep.json`).
