#!/usr/bin/env bash
base="$(cd "$(dirname "$0")" && pwd)"
for d in "$@"; do
  cd "$base/$d" && rm -rf build/hls && bash slurm.sh csynth
done
squeue -u $USER
