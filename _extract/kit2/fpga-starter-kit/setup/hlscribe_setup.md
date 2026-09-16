# hlscribe — Setup & Usage (FPGA Monsoon 2026)

`hlscribe` is a small HLS project tool. It has an AI code-generation side, but
**in this course we use only the non-AI parts**: scaffolding a project/design,
placing your hand-written kernel, and running builds (C-sim → synthesis →
co-sim) on the Slurm cluster, plus a mechanical design-space sweep.

> **Prerequisite:** finish the U50/Vitis environment setup first —
> see [fpga_setup.md](./fpga_setup.md). Builds submit to Slurm from `rce`.

---

## 1. Install

`hlscribe` ships as a prebuilt wheel — you do **not** clone any repository. Use a
Python 3.11+ virtual environment and `pip install` the wheel from whichever
source you have:

```bash
# The wheel is bundled in the starter kit (setup/hlscribe/):
pip install setup/hlscribe/hlscribe-0.1.0-py3-none-any.whl

# ...or install it straight from the shared copy on RCE:
pip install /scratch/fpga-monsoon26/hlscribe/hlscribe-0.1.0-py3-none-any.whl

hlscribe --version          # sanity check
```

> **Note:** the install pulls dependencies (`anthropic`, `openai`, etc.) from
> PyPI, so run it on a machine with internet — the RCE login node has it. On
> RCE, create the venv from the Python module first (see
> [rce-starter-kit.md §6](./rce-starter-kit.md)).

---

## 2. One-time configuration (`~/.hlscribe/`)

hlscribe reads two global files from your home directory.

### 2a. `~/.hlscribe/.env` — a key must be present

```bash
mkdir -p ~/.hlscribe
nano ~/.hlscribe/.env
```

```ini
# Any ONE of these is enough. We do NOT use the AI features in this course,
# so the value can be a placeholder — but the line must exist, or every
# hlscribe command (including plain builds) will exit with
# "ANTHROPIC_API_KEY not set".
ANTHROPIC_API_KEY=sk-ant-not-used
```

> **Why:** `hlscribe` loads config on *every* command and refuses to run without
> a provider key, even for commands that never call an LLM (`init-design`,
> `build`, `jobs`, `results`). A dummy string satisfies this. Only
> `hlscribe check-config` and the AI commands make a *live* API call and need a
> real key — so **skip `check-config`** unless you have a genuine key.

### 2b. `~/.hlscribe/config.yaml` — Slurm cluster (rce)

```yaml
slurm:
  host: rce
  user: <your-username>      # your rce login
  partition: debug
  nodes: 4
  cpus: 8
  mem: 32G
  time: "02:00:00"
```

This is what makes `hlscribe build <stage> --slurm` submit jobs over SSH to
`<user>@rce`. Make sure `ssh <user>@rce` works without an interactive password
(set up an SSH key) so batch submission is non-interactive.

---

## 3. Project & design layout

One **project** per kernel topic; one or more **designs** (variants) inside it.

```bash
# Create the project (U50, 300 MHz ≈ 3.3 ns clock)
hlscribe init-project vadd --platform u50 --clock 3.3ns --top vadd
cd vadd

# Create a design variant to work in
hlscribe init-design baseline
cd baseline
```

Resulting layout (the parts you touch are marked ←):

```
vadd/                        # project root
├── hlscribe.yaml            # platform, clock, top_function (shared defaults)
├── specs/                   # optional written specs (not required for our flow)
└── baseline/                # a design variant (from init-design)
    ├── Makefile             # marks the design; drives the build
    ├── hlscribe.yaml        # design-level config
    ├── src/
    │   ├── kernel.cpp       ← your kernel goes here
    │   ├── kernel.h         ← (optional) shared declarations
    │   ├── host.cpp           host/testbench harness (for hw/emulation)
    │   └── kernel_dse.cpp     pragma-free copy (generated in the DSE step)
    ├── test/
    │   └── kernel_tb.cpp    ← your C-sim testbench goes here
    ├── configs/              DSE .cfg files (generated)
    ├── scripts/              build scripts (sourced env, v++/vitis-run calls)
    └── artifacts/logs/       build + Slurm logs
```

Set the top function name in `hlscribe.yaml` (`top_function: vadd`) so it matches
the `extern "C"` entry point in your kernel.

---

## 4. Placing your kernel

Since we write kernels by hand, put your source directly in the design:

```bash
# edit src/kernel.cpp and test/kernel_tb.cpp
```

Or import files written elsewhere:

```bash
hlscribe import-kernel path/to/vadd.cpp path/to/vadd.h
hlscribe list-imports          # confirm what landed in src/
```

---

## 5. Build flow (Slurm on rce)

Run these **from inside the design directory** (the one with the `Makefile`).

```bash
hlscribe build csim   --slurm     # C simulation — functional check (1–5 min)
hlscribe build csynth --slurm     # HLS synthesis — RTL + timing/area (5–30 min)
hlscribe build cosim  --slurm     # RTL co-simulation vs testbench (10–60 min)
```

Track and inspect jobs:

```bash
hlscribe jobs                     # running / completed jobs for this design
hlscribe jobs --cancel <jobid>    # cancel one
hlscribe logs baseline csim           # view a stage's build log
hlscribe logs baseline csynth --errors  # errors only
```

View synthesis results:

```bash
hlscribe results                  # metrics table (LUT/FF/DSP/BRAM, II, latency)
hlscribe results --best           # best config so far
hlscribe results --sort-by latency
```

> Local builds (without `--slurm`) also work if Vitis is on your box, but this
> course standardizes on Slurm so everyone shares the licensed toolchain.

---

## 6. Micro-DSE — sweeping pragma configurations (later in the course)

Once a kernel builds and passes C-sim, you can explore knob settings (II, unroll
factor, array partitioning) mechanically, without editing the source each time:

```bash
# 1. Strip micro-knob pragmas from the working kernel into a pragma-free
#    kernel_dse.cpp + a baseline_dse.cfg (regex only, no LLM: --nollm)
hlscribe codegen --dse --strip --nollm

# 2. Generate a mechanical sweep of configs (e.g. II = 1, 2, 4)
hlscribe gen-configs --sweep 'ii=1,2,4'

# 3. Build every config on Slurm
hlscribe build csynth --sweep --slurm

# 4. Compare
hlscribe results --sort-by latency --best
```

Structural pragmas (`INTERFACE`, `DATAFLOW`, `STREAM`, `DEPENDENCE`,
`LOOP_TRIPCOUNT`, `INLINE OFF`) stay in the code; only the tunable micro-knobs
(`PIPELINE`, `UNROLL`, `ARRAY_PARTITION`, `INLINE`) move into `.cfg` directives
that the sweep varies.

---

## 7. Command quick reference

| Command | Purpose |
|---|---|
| `hlscribe init-project <n> --platform u50 --clock 3.3ns --top <fn>` | Create a project |
| `hlscribe init-design <n>` | Create a design variant inside a project |
| `hlscribe import-kernel <files>` | Copy hand-written sources into `src/` |
| `hlscribe build csim\|csynth\|cosim --slurm` | Run a build stage on the cluster |
| `hlscribe jobs [--cancel <id>]` | List / cancel Slurm jobs |
| `hlscribe logs <sol> <stage> [--errors]` | View build logs |
| `hlscribe results [--best] [--sort-by …]` | Synthesis metrics |
| `hlscribe codegen --dse --strip --nollm` | Make a pragma-free DSE kernel |
| `hlscribe gen-configs --sweep 'ii=1,2,4'` | Mechanical config sweep |

**Build stages:** `csim` (functional) → `csynth` (synthesis) → `cosim` (RTL check).

---

## 8. Troubleshooting

| Symptom | Fix |
|---|---|
| `ANTHROPIC_API_KEY not set` on any command | Add a key line to `~/.hlscribe/.env` (§2a) — a placeholder is fine for builds |
| `Not inside a design directory (no Makefile found)` | `cd` into the design dir created by `init-design` |
| `Slurm not configured. Set slurm.host …` | Fill in `~/.hlscribe/config.yaml` (§2b) |
| Slurm submit hangs on a password prompt | Set up passwordless SSH to `<user>@rce` |
| License / platform errors in build logs | Re-check the U50 environment in [fpga_setup.md](./fpga_setup.md) (VPN + `LM_LICENSE_FILE`) |
| `check-config` fails on API connection | Expected without a real key — we don't use it; ignore |
