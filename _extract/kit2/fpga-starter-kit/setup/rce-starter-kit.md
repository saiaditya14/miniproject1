# RCE Starter Kit — FPGA / HLS on the IIIT-H Cluster

Everything you need to go from *"I have an RCE account"* to *"I ran C-sim,
synthesis, and co-simulation for an HLS kernel on the cluster."* Every command
here was run and verified on `rce.iiit.ac.in` with Vitis 2025.1 and the Alveo
**U50** platform.

This kit is distributed as `fpga-starter-kit.zip` on the course Moodle. The
companion runnable example is [`../labs/hello-hls/`](../labs/hello-hls/) — unzip
the kit (§1), run it, and you have passed the smoke test.

**What runs where**

| Stage | Where | Needs the FPGA? |
|---|---|---|
| csim, csynth, cosim | any cluster node (Slurm) | no |
| `v++` build → `.xclbin` (`hw_emu`, `hw`) | any cluster node (Slurm) | no |
| **executing** a `hw` `.xclbin` | **node08 only** (has the U50) | **yes** |

Building the bitstream is just software — it runs on the cluster. Only the final
*run on real hardware* needs the U50, which lives on **node08** (outside Slurm).
A job-submission path for node08 will be provided later; this kit covers
everything up to it.

---

## 0. Your account at a glance

| | |
|---|---|
| Login host | `rce.iiit.ac.in` (needs IIIT intranet or **VPN**) |
| Scheduler | Slurm — partition **`debug`**, nodes `node01`–`node08` |
| Max vcores | **8** |
| Max RAM | **32 GB** |
| Disk quota | **20 GB** (in `$HOME`) |
| Account expiry | **Dec 15** — back up your work before then |
| Toolchain | Vitis **2025.1**, `/home/Xilinx/2025.1` (shared on every node) |
| FPGA | Alveo **U50** on **node08** (drained from Slurm; hardware runs only) |

Keep every job within **8 CPUs / 32 GB** — that is what the `--cpus-per-task`
and `--mem` flags below request.

---

## 1. Log in and get the kit

**Log in.** From a machine on the IIIT intranet (or on the IIIT **VPN**):

```bash
ssh <your-username>@rce.iiit.ac.in
```

`rce` is the **login/head node** — you edit files and submit jobs here, but you
do **not** run heavy builds directly on it. Builds go to compute nodes through
Slurm (next sections).

**Set up passwordless SSH** (recommended — makes `scp` below painless, and
hlscribe needs it later):

```bash
ssh-keygen -t ed25519          # accept defaults, empty passphrase is fine
ssh-copy-id <your-username>@rce.iiit.ac.in
```

**Get the starter kit onto RCE.** Download `fpga-starter-kit.zip` from the
course Moodle to your laptop, copy it up, then unzip it in your RCE home:

```bash
# on your laptop (after downloading the zip from Moodle)
scp fpga-starter-kit.zip <your-username>@rce.iiit.ac.in:~/

# then, logged in on rce
unzip fpga-starter-kit.zip        # creates ~/fpga-starter-kit/
cd fpga-starter-kit
ls                                # README.md  setup/  labs/
```

You are now reading `setup/rce-starter-kit.md` from inside that folder. The
runnable smoke test is in `labs/hello-hls/`. Everything below runs from there.

---

## 2. Load the toolchain

One command puts Vitis, `v++`, XRT, and the U50 platform on your `PATH`. The
starter kit ships it as [`labs/hello-hls/env.sh`](../labs/hello-hls/env.sh):

```bash
source /home/Xilinx/2025.1/Vitis/settings64.sh          # Vitis HLS + v++
source /opt/xilinx/xrt/setup.sh                          # XRT (host / hw_emu / hw)
export PLATFORM_REPO_PATHS=/opt/xilinx/platforms:/home/Xilinx/opt/xilinx/platforms
export PLATFORM=xilinx_u50_gen3x16_xdma_5_202210_1
export PART=xcu50-fsvh2104-2-e
```

> **Platform note.** The U50 `.xpfm` is **not** in `/opt/xilinx/platforms` on this
> cluster; it lives on the shared NFS copy under `/home/Xilinx/opt/xilinx/platforms`.
> Because `/home` is mounted on every node, the two paths above resolve
> everywhere. You only need them for `hw`/`hw_emu` builds — csim/csynth/cosim work
> without them.

Verify:

```bash
which v++ vitis-run      # both should resolve under /home/Xilinx/2025.1
```

> **Vitis 2025.1 uses a config-file flow**, not classic `vitis_hls -f run.tcl`.
> The HLS stages are invoked as `vitis-run --mode hls --csim/--cosim` and
> `v++ -c --mode hls`, all driven by a `.cfg` file (see `hls.cfg`).

---

## 3. Get a node

Two ways. Use **interactive** while you are debugging a design; use **batch**
for anything that takes minutes and for everything you want to walk away from.

### 3a. Interactive — a shell on a compute node

```bash
srun --partition=debug --cpus-per-task=8 --mem=16G --time=01:00:00 --pty bash
```

You land on a compute node (e.g. `node07`). `source env.sh`, run tools by hand,
`exit` to release it. `salloc` (same flags, no `--pty bash`) is the alternative
if you prefer to keep your prompt on `rce` and `srun` individual steps into the
allocation.

> **Release interactive nodes when you stop typing.** They hold your CPU/memory
> reservation until you `exit`.

### 3b. Batch — submit and walk away

Write an `#SBATCH`-headed script (see `smoke.sbatch`) and submit it:

```bash
sbatch smoke.sbatch          # prints: Submitted batch job <id>
```

Output streams to the `--output` file (`smoke_<id>.log`).

### Monitoring (either way)

```bash
squeue -u $USER                                  # your queued/running jobs
squeue -u $USER -o "%.8i %.10j %.9T %.8M %R"     # id / name / state / time / node
scancel <jobid>                                  # cancel one
sacct -j <jobid> --format=JobID,State,Elapsed,MaxRSS,AllocCPUS   # after it ends
```

---

## 4. The smoke test — csim, csynth, cosim

The kit is already on the cluster from §1. Move into it — this is the exact
design that was verified on the cluster:

```bash
cd ~/fpga-starter-kit/labs/hello-hls
```

### Batch (recommended first run)

```bash
sbatch smoke.sbatch
squeue -u $USER                        # ~2 min on an idle node
grep -E "TEST PASSED|finished: PASS" smoke_*.log
```

A healthy run shows, in `smoke_<id>.log`:

```
### [1/3] CSIM ...   vadd testbench: N=1024   TEST PASSED
### [2/3] CSYNTH ... INFO: [HLS 200-802] Generated output file hls_work/vadd.zip
### [3/3] COSIM ...  *** C/RTL co-simulation finished: PASS ***
```

### Interactive (run the three stages yourself)

```bash
srun --partition=debug --cpus-per-task=8 --mem=16G --time=01:00:00 --pty bash
source env.sh
vitis-run --mode hls --csim  --config hls.cfg --work_dir hls_work   # ~10 s: TEST PASSED
v++ -c       --mode hls        --config hls.cfg --work_dir hls_work   # ~45 s: RTL + reports
vitis-run --mode hls --cosim --config hls.cfg --work_dir hls_work   # ~50 s: PASS
exit
```

### Reading the results

After **csynth**, open the report:

```bash
less hls_work/hls/syn/report/vadd_csynth.rpt
```

The three numbers that matter for this course, for the verified baseline:

| Metric | Where in the report | Baseline value |
|---|---|---|
| **Timing** — estimated vs target clock | `+ Timing` | 2.805 ns ≤ 3.30 ns target ✅ |
| **II** — initiation interval of the loop | `+ Loop` → `vadd_loop` | **1** (one element/cycle) |
| **Resources** — LUT/FF/DSP/BRAM | `Utilization Estimates` | ~2210 LUT, 2205 FF, 2 BRAM |

Estimated clock under target and II = 1 is exactly what you want from the
pipelined `vadd` loop. Later labs make these numbers move.

---

## 5. Building an `.xclbin` (hw_emu / hw)

`v++` turns the kernel into a device image. Two targets, both built on the
cluster (no FPGA needed to build):

```bash
cd ~/hello-hls
sbatch --export=ALL,TGT=hw_emu build_xclbin.sbatch    # emulation image, ~4 min
sbatch --export=ALL,TGT=hw     build_xclbin.sbatch    # real bitstream, ~1 h (place & route)
```

Output lands in `build_hw_emu/vadd_hw_emu.xclbin` / `build_hw/vadd_hw.xclbin`.

- **`hw_emu`** — a fast, cycle-approximate model. You can *run* it in software
  emulation on any cluster node (needs a host app + `emconfigutil` — added with
  the labs).
- **`hw`** — the real bitstream. Building it is a ~1-hour Vivado place-and-route
  on the cluster; **running** it needs the U50 on node08.

> **One `v++` link per directory.** `v++` writes `_x/` and Vivado run dirs into
> the current directory; two links running in the same directory corrupt each
> other (`ERROR: [SYSTEM_LINK 82-*]` / `cf2bd failed`). `build_xclbin.sbatch`
> already isolates each target in its own `build_<TGT>/` dir — keep that pattern.

Running a `hw` `.xclbin` on the U50 opens the card **by BDF**
(`0000:02:00.1`), never by device index — node08 also hosts a VCK5000 that
enumerates as index 0. The hardware-run path is provided separately.

---

## 6. hlscribe — the course build driver (optional for the smoke test)

The labs are driven with `hlscribe` (non-AI features only): it scaffolds a
project, holds your kernel, and submits csim/csynth/cosim to Slurm for you, plus
a mechanical pragma sweep. It ships as a prebuilt wheel — bundled in this starter
kit under `setup/hlscribe/`, and also on RCE at
`/scratch/fpga-monsoon26/hlscribe/`. No repository to clone. The smoke test above
does **not** need it — but the labs assume it. Full instructions:
[`hlscribe_setup.md`](./hlscribe_setup.md). The one RCE-specific gotcha:

```bash
# system python is 3.6 — too old. Use the module + a venv:
module load python/3.12.5
python3 -m venv ~/venvs/hlscribe && source ~/venvs/hlscribe/bin/activate
# install the wheel (bundled, or from /scratch), then configure ~/.hlscribe (§2):
pip install /scratch/fpga-monsoon26/hlscribe/hlscribe-0.1.0-py3-none-any.whl
# Point slurm.host: rce and partition: debug.
```

Once set up, from inside a design directory:

```bash
hlscribe build csim   --slurm
hlscribe build csynth --slurm
hlscribe build cosim  --slurm
hlscribe results --best
```

---

## 7. Housekeeping (20 GB quota, Dec 15 expiry)

- **Watch your disk.** `du -sh ~/* | sort -h` — HLS build dirs (`hls_work/`,
  `_x/`, `build_hw*/`) are the usual space hogs. Delete them when done:
  `rm -rf hls_work _x build_hw* .Xil`. Check usage with `df -h $HOME` /
  `du -sh ~`.
- **Stay within 8 CPU / 32 GB per job** — the `--cpus-per-task=8 --mem` flags in
  the provided scripts already do.
- **Back up before Dec 15.** Accounts expire; `git push` your work or `scp` it
  off the cluster.

---

## Quick reference

```bash
scp fpga-starter-kit.zip <user>@rce.iiit.ac.in:~/            # (from laptop) upload the Moodle zip
ssh <user>@rce.iiit.ac.in                                    # log in
unzip fpga-starter-kit.zip && cd fpga-starter-kit/labs/hello-hls   # unpack + enter the kit
source env.sh                                                # load Vitis 2025.1 + U50
srun -p debug --cpus-per-task=8 --mem=16G --time=1:00:00 --pty bash   # interactive node
sbatch smoke.sbatch                                          # batch: csim->csynth->cosim
squeue -u $USER   /   scancel <id>   /   sacct -j <id>       # monitor / cancel / audit
sbatch --export=ALL,TGT=hw_emu build_xclbin.sbatch           # build emulation .xclbin
sbatch --export=ALL,TGT=hw     build_xclbin.sbatch           # build real bitstream (~1 h)
```

| Symptom | Fix |
|---|---|
| `ssh: connect ... timed out` | Get on the IIIT intranet / VPN first |
| `which v++` → not found | `source .../2025.1/Vitis/settings64.sh` (or `env.sh`) |
| hw/hw_emu link: `Cannot find platform` | export `PLATFORM_REPO_PATHS` (§2) |
| `ERROR: [SYSTEM_LINK 82-*]` / `cf2bd failed` | two `v++` links in one dir — use a per-target build dir (§5) |
| job stuck `PENDING` | nodes busy; `squeue` to see, or lower `--mem`/`--cpus-per-task` |
| `DiskQuotaExceeded` | delete `hls_work/ _x/ build_hw*/` (§7) |
