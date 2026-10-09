[cs2401.02@rce rce_upload]$ cd ~/rce_upload; chmod +x report.sh; ./report.sh fpga_r12g32 fpga_r16g32_q1 fpga_r16g32_q3; for d in fpga_r16g32_q1 fpga_r16g32_q3; do echo "## $d"; sed -n 8,14p $d/build/hls/hls/sim/report/gemm_cosim.rpt; done
########## fpga_r12g32
 #define USE_DATAFLOW 1
2
Resolution: For help on HLS 200-871 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug144
WARNING: [HLS 200-871] Estimated clock period (2.729 ns) exceeds the target (target clock period: 3.330 ns, clock uncert
        'fadd' operation 32 bit ('cur', ../src/kernel.cpp:277) [14356]  (2.342 ns)
        'store' operation 0 bit ('sh_3064_write_ln277', ../src/kernel.cpp:277) of variable 'cur', ../src/kernel.cpp:277 on local variable 'sh_3064' [14357]  (0.387 ns)
WARNING: [HLS 200-1016] The critical path in module 'tl_compute8_Pipeline_VITIS_LOOP_241_6' consists of the following:
|Total                |      807|  2051|   565626|  304636|    0|
|Utilization SLR (%)  |       60|    68|       64|      69|    0|
    |ap_clk  |  3.33 ns|  3.491 ns|     0.90 ns|
########## fpga_r16g32_q1
 #define USE_DATAFLOW 1
2
grep: build/logs/csynth-*.log: No such file or directory
grep: build/logs/csynth-*.log: No such file or directory
grep: build/logs/csynth-*.log: No such file or directory
|Total                |      871|  2691|   610899|  359313|    0|
|Utilization SLR (%)  |       64|    90|       70|      82|    0|
    |ap_clk  |  3.33 ns|  3.491 ns|     0.90 ns|
########## fpga_r16g32_q3
 #define USE_DATAFLOW 1
2
grep: build/logs/csynth-*.log: No such file or directory
grep: build/logs/csynth-*.log: No such file or directory
grep: build/logs/csynth-*.log: No such file or directory
|Total                |      871|  2691|   610899|  359313|    0|
|Utilization SLR (%)  |       64|    90|       70|      82|    0|
    |ap_clk  |  3.33 ns|  3.491 ns|     0.90 ns|
## fpga_r16g32_q1
|          |          |      min      |      avg      |      max      |      min      |      avg      |      max      |                      |
+----------+----------+-----------------------------------------------+-----------------------------------------------+----------------------+
|      VHDL|        NA|             NA|             NA|             NA|             NA|             NA|             NA|                    NA|
|   Verilog|      Pass|           1518|           2391|           3483|           1540|           1867|           2195|                  7218|
+----------+----------+-----------------------------------------------+-----------------------------------------------+----------------------+
## fpga_r16g32_q3
|          |          |      min      |      avg      |      max      |      min      |      avg      |      max      |                      |
+----------+----------+-----------------------------------------------+-----------------------------------------------+----------------------+
|      VHDL|        NA|             NA|             NA|             NA|             NA|             NA|             NA|                    NA|
|   Verilog|      Fail|           4468|           6081|           8026|           4490|           5131|           5772|                 18288|
+----------+----------+-----------------------------------------------+-----------------------------------------------+----------------------+
[cs2401.02@rce rce_upload]$ 