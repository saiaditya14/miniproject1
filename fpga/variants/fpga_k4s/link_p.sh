#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
make csynth
source ./env.sh >/dev/null
mkdir -p build/hw
cd build/hw
v++ -l -t hw --platform $PLATFORM $PWD/../hls/gemm.xo -o gemm.xclbin --config $PWD/../../vivado_opt.cfg
echo "bitstream: build/hw/gemm.xclbin"
