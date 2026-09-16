# hello-hls — the RCE smoke test

The smallest end-to-end HLS design: a `vadd` kernel (`out[i] = in1[i] + in2[i]`)
plus a testbench. Use it to prove your RCE toolchain works **before** you touch a
real lab. Every stage here has been run and verified on the cluster.

Full walkthrough (login, nodes, quotas, hlscribe): **[`../../setup/rce-starter-kit.md`](../../setup/rce-starter-kit.md)**.

## Files

| File | Purpose |
|---|---|
| `src/vadd.cpp`         | the kernel |
| `test/vadd_tb.cpp`     | C/RTL testbench (returns 0 = PASS) |
| `hls.cfg`              | HLS config (part, top, sources, clock) |
| `env.sh`               | `source` it to load Vitis 2025.1 + XRT + U50 platform |
| `smoke.sbatch`         | batch job: csim → csynth → cosim |
| `build_xclbin.sbatch`  | batch job: `v++` build to `.xclbin` (`hw_emu` or `hw`) |

## Quickest path (batch)

```bash
# on rce, from this directory
sbatch smoke.sbatch
squeue -u $USER                 # wait for it to leave the queue (~2 min)
grep -E "TEST PASSED|PASS" smoke_*.log
```

Expected: `TEST PASSED` (csim), then `C/RTL co-simulation finished: PASS` (cosim).
Synthesis metrics land in `hls_work/hls/syn/report/vadd_csynth.rpt`.

## Interactive path (one node, run stages by hand)

```bash
srun --partition=debug --cpus-per-task=8 --mem=16G --time=01:00:00 --pty bash
source env.sh
vitis-run --mode hls --csim  --config hls.cfg --work_dir hls_work   # functional
v++ -c       --mode hls        --config hls.cfg --work_dir hls_work   # synthesis
vitis-run --mode hls --cosim --config hls.cfg --work_dir hls_work   # RTL check
exit
```

## Build an .xclbin (no FPGA needed to build)

```bash
sbatch --export=ALL,TGT=hw_emu build_xclbin.sbatch   # emulation image, ~4 min
sbatch --export=ALL,TGT=hw     build_xclbin.sbatch   # real bitstream, ~1 h (P&R)
```

`hw_emu`/`hw` land in `build_hw_emu/` / `build_hw/`. Building the bitstream runs
entirely on the cluster; only *running* a `hw` `.xclbin` needs the U50 on node08.

> Do not run two `v++` links in the same directory at once — `v++` writes `_x/`
> into the working dir and concurrent links corrupt each other. `build_xclbin.sbatch`
> already isolates each target in its own `build_<TGT>/` dir.
