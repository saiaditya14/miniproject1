#!/usr/bin/env bash
# smoke.sbatch — run the full HLS front-end (csim -> csynth -> cosim) as a
# batch job on the RCE Slurm cluster.  Submit from this directory:
#
#     sbatch smoke.sbatch
#     squeue -u $USER            # watch it
#     tail -f smoke_<jobid>.log  # follow the output
#
# Expected result: three stages, ending in "TEST PASSED" and
# "C/RTL co-simulation finished: PASS".  Runtime ~2 min on an idle node.

#SBATCH --job-name=hls-smoke
#SBATCH --partition=debug
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8          # within the 8-vcore student cap
#SBATCH --mem=16G                  # within the 32 GB cap
#SBATCH --time=00:30:00
#SBATCH --output=smoke_%j.log

set -eo pipefail
cd "$SLURM_SUBMIT_DIR"
source "$SLURM_SUBMIT_DIR/env.sh" >/dev/null

echo "### Host: $(hostname)   Job: $SLURM_JOB_ID"
echo "### [1/3] CSIM  (functional C simulation)"
vitis-run --mode hls --csim  --config hls.cfg --work_dir hls_work
echo "### [2/3] CSYNTH (HLS synthesis -> RTL + timing/area)"
v++ -c       --mode hls        --config hls.cfg --work_dir hls_work
echo "### [3/3] COSIM (RTL co-simulation vs testbench)"
vitis-run --mode hls --cosim --config hls.cfg --work_dir hls_work
echo "### DONE — synthesis report: hls_work/hls/syn/report/vadd_csynth.rpt"
