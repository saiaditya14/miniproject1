#!/usr/bin/env bash
for d in "$@"; do
  echo "########## $d"
  cd "$(cd "$(dirname "$0")" && pwd)/$d"
  grep -E "^#define (RT|NT|LACC|RG|ACC_STYLE|USE_DATAFLOW) " src/kernel_cfg.h | tr '\n' ' '; echo
  grep -c "style = frp" src/kernel.cpp
  grep -hE "Final II = [2-9]|Final II = [0-9][0-9]" build/logs/csynth-*.log | grep -v "VITIS_LOOP_10_3" | cut -c1-200 | sort -u
  grep -hE "HLS 200-871" build/logs/csynth-*.log | sort -u | cut -c1-120
  grep -h -A3 "HLS 200-1016" build/logs/csynth-*.log | grep -E "critical path in module|operation" | cut -c1-170 | sort -u | head -8
  sed -n '/== Utilization Estimates/,/+ Detail/p' build/hls/hls/syn/report/gemm_csynth.rpt | grep -E "^\|Total|Utilization SLR"
  sed -n '/== Performance Estimates/,/+ Latency/p' build/hls/hls/syn/report/gemm_csynth.rpt | grep ap_clk
  cd - >/dev/null
done
