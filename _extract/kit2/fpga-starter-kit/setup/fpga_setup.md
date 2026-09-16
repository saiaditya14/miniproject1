# FPGA Setup Guide — Alveo U50 (IIIT-H)

This guide explains how to access the IIIT-H FPGA nodes, configure the
environment, and set up **Vitis** for the **Alveo U50** board used in this course.
It is specific to the IIIT-H environment (VPN, RCE, and license server).

> This course targets the **U50** only. The nodes also host a VCK5000 (Vitis
> 2022.2); ignore it unless told otherwise.

---

## 1. Accessing RCE and Nodes

You must be on the **IIIT intranet** or connected via **IIIT VPN**.

```bash
ssh username@rce           # login/head node — where we submit Slurm jobs from
```

Builds in this course run on the Slurm cluster reachable from `rce`
(see [hlscribe_setup.md](./hlscribe_setup.md)), so you normally do **not**
need to hop onto an individual node by hand.

---

## 2. Vitis Installation

* **Vitis 2025.1**: `/home/Xilinx/2025.1/`   ← use this for the U50

---

## 3. Platform Files

On this cluster the U50 build platform (`.xpfm`) is **not** in `/opt/xilinx/platforms/`
— it lives on the shared NFS mount:

```
/home/Xilinx/opt/xilinx/platforms/xilinx_u50_gen3x16_xdma_5_202210_1
```

Because `/home` is shared across `rce` and every node, this path resolves
everywhere. `hlscribe`'s `env_setup.sh` already searches it, so you normally
don't set anything. If you build outside hlscribe, point v++ at it:

```bash
export PLATFORM_REPO_PATHS=/opt/xilinx/platforms:/home/Xilinx/opt/xilinx/platforms
```

* **U50** platform → `xilinx_u50_gen3x16_xdma_5_202210_1`
* Device part → `xcu50-fsvh2104-2-e`

Verify the board is visible (run on **node08**, which has the U50):

```bash
xbutil examine                     # lists all cards + BDFs
xbutil examine -d 0000:02:00.1     # the U50 specifically
```

> **Two cards on node08.** It hosts both a VCK5000 (`0000:82:00.1`) and the U50
> (`0000:02:00.1`), and XRT enumerates the **VCK5000 as device index 0**. Your XRT
> host must open the U50 **by BDF** (`xrt::device("0000:02:00.1")`), never by
> index 0 — otherwise loading a U50 xclbin fails with `err = -22 / failed to load
> xclbin: Invalid argument`. See the vadd lab for a worked example.

---

## 4. Environment Setup (U50)

Add the following to your `~/.bashrc` on `rce` (and any build node):

```bash
export LM_LICENSE_FILE=2100@rce.iiit.ac.in
export LD_LIBRARY_PATH=/opt/xilinx/xrt/lib:$LD_LIBRARY_PATH
source /opt/xilinx/xrt/setup.sh
source /home/Xilinx/2025.1/Vitis/settings64.sh
export PLATFORM_REPO_PATHS=/opt/xilinx/platforms
```

For scripted/Slurm builds you will also want these exported (hlscribe's build
scripts source the equivalent — see the hlscribe setup doc):

```bash
export PLATFORM=xilinx_u50_gen3x16_xdma_5_202210_1
export PART=xcu50-fsvh2104-2-e
```

---

## 5. Notes

* Connect to **VPN/intranet** before using the license server (`2100@rce.iiit.ac.in`).
* Keep `~/.bashrc` updated so you don't repeat environment setup each session.
* `xbutil examine` reports board status, memory, and health.

---

**Next:** [hlscribe_setup.md](./hlscribe_setup.md) — install and use the `hlscribe`
tool to scaffold designs and run builds on this cluster.
