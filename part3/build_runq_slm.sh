#!/usr/bin/env bash
set -euo pipefail
L="${1:?path to llama2.c}"
H="$(cd "$(dirname "$0")/.." && pwd)"
F="-O3 -w -Wno-error=incompatible-pointer-types -Wno-error=implicit-function-declaration"
gcc $F -o "$L/runq_slm.exe" -Dmatmul=matmul_golden "$L/runq.c" "$L/win.c" "$H/part3/int8_backend.c" "$H/student_gemm_int8.c" -I"$H/part3" -lm
