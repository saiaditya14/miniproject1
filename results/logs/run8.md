[cs2401.02@rce rce_upload]$ cd ~/rce_upload; squeue -u $USER; chmod +x ./report.sh; ./report.sh fpga_v6x fpga_v12x fpga_g16 fpga_g32; for d in fpga_v5b_q3 fpga_v5a_q3; do echo "## $d"; sed -n 8,14p $d/build/hls/hls/sim/report/gemm_cosim.rpt; done
             JOBID PARTITION     NAME     USER ST       TIME  NODES NODELIST(REASON)
########## fpga_v6x
#define RT 16 #define NT 32 #define LACC 8 #define RG 8 #define ACC_STYLE 0 #define USE_DATAFLOW 1 
2
Resolution: For help on HLS 200-871 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug144
WARNING: [HLS 200-871] Estimated clock period (2.729 ns) exceeds the target (target clock period: 3.330 ns, clock uncert
        'fadd' operation 32 bit ('acc', ../src/kernel.cpp:65->../src/kernel.cpp:106->../src/kernel.cpp:103) [429]  (2.342 ns)
        'fadd' operation 32 bit ('cur', ../src/kernel.cpp:266) [19092]  (2.342 ns)
        'store' operation 0 bit ('acc_7_write_ln65', ../src/kernel.cpp:65->../src/kernel.cpp:106->../src/kernel.cpp:103) of variable 'acc', ../src/kernel.cpp:65->../src/kernel.c
        'store' operation 0 bit ('sh_4088_write_ln185', ../src/kernel.cpp:185) of variable 'cur', ../src/kernel.cpp:266 on local variable 'sh', ../src/kernel.cpp:185 [19096]  (0
WARNING: [HLS 200-1016] The critical path in module 'gv_compute5_Pipeline_VITIS_LOOP_49_2' consists of the following:
WARNING: [HLS 200-1016] The critical path in module 'tl_compute8_Pipeline_VITIS_LOOP_230_6' consists of the following:
|Total                |      487|  2691|   607479|  356098|    0|
|Utilization SLR (%)  |       36|    90|       69|      81|    0|
    |ap_clk  |  3.33 ns|  3.491 ns|     0.90 ns|
########## fpga_v12x
#define RT 12 #define NT 32 #define LACC 8 #define RG 8 #define ACC_STYLE 0 #define USE_DATAFLOW 1 
2
Resolution: For help on HLS 200-871 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug144
WARNING: [HLS 200-871] Estimated clock period (2.729 ns) exceeds the target (target clock period: 3.330 ns, clock uncert
        'fadd' operation 32 bit ('acc', ../src/kernel.cpp:65->../src/kernel.cpp:106->../src/kernel.cpp:103) [429]  (2.342 ns)
        'fadd' operation 32 bit ('cur', ../src/kernel.cpp:266) [14356]  (2.342 ns)
        'store' operation 0 bit ('acc_7_write_ln65', ../src/kernel.cpp:65->../src/kernel.cpp:106->../src/kernel.cpp:103) of variable 'acc', ../src/kernel.cpp:65->../src/kernel.c
        'store' operation 0 bit ('sh_3064_write_ln266', ../src/kernel.cpp:266) of variable 'cur', ../src/kernel.cpp:266 on local variable 'sh_3064' [14357]  (0.387 ns)
WARNING: [HLS 200-1016] The critical path in module 'gv_compute5_Pipeline_VITIS_LOOP_49_2' consists of the following:
WARNING: [HLS 200-1016] The critical path in module 'tl_compute8_Pipeline_VITIS_LOOP_230_6' consists of the following:
|Total                |      423|  2051|   562206|  301421|    0|
|Utilization SLR (%)  |       31|    68|       64|      69|    0|
    |ap_clk  |  3.33 ns|  3.491 ns|     0.90 ns|
########## fpga_g16
 #define USE_DATAFLOW 1
0
Resolution: For help on HLS 200-871 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug144
WARNING: [HLS 200-871] Estimated clock period (2.729 ns) exceeds the target (target clock period: 3.330 ns, clock uncert
        'fadd' operation 32 bit ('cur', ../src/kernel.cpp:266) [9628]  (2.342 ns)
        'store' operation 0 bit ('sh_2040_write_ln266', ../src/kernel.cpp:266) of variable 'cur', ../src/kernel.cpp:266 on local variable 'sh_2040' [9629]  (0.387 ns)
WARNING: [HLS 200-1016] The critical path in module 'tl_compute8_Pipeline_VITIS_LOOP_230_6' consists of the following:
|Total                |      487|  1411|   387299|  214899|    0|
|Utilization SLR (%)  |       36|    47|       44|      49|    0|
    |ap_clk  |  3.33 ns|  3.491 ns|     0.90 ns|
########## fpga_g32
 #define USE_DATAFLOW 1
0
Resolution: For help on HLS 200-871 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug144
WARNING: [HLS 200-871] Estimated clock period (2.729 ns) exceeds the target (target clock period: 3.330 ns, clock uncert
        'fadd' operation 32 bit ('cur', ../src/kernel.cpp:266) [9628]  (2.342 ns)
        'store' operation 0 bit ('sh_2040_write_ln266', ../src/kernel.cpp:266) of variable 'cur', ../src/kernel.cpp:266 on local variable 'sh_2040' [9629]  (0.387 ns)
WARNING: [HLS 200-1016] The critical path in module 'tl_compute8_Pipeline_VITIS_LOOP_230_6' consists of the following:
|Total                |      743|  1411|   389969|  217412|    0|
|Utilization SLR (%)  |       55|    47|       44|      49|    0|
    |ap_clk  |  3.33 ns|  3.491 ns|     0.90 ns|
## fpga_v5b_q3
|          |          |      min      |      avg      |      max      |      min      |      avg      |      max      |                      |
+----------+----------+-----------------------------------------------+-----------------------------------------------+----------------------+
|      VHDL|        NA|             NA|             NA|             NA|             NA|             NA|             NA|                    NA|
|   Verilog|      Pass|           3832|           4800|           5974|           3854|           4235|           4616|                 14444|
+----------+----------+-----------------------------------------------+-----------------------------------------------+----------------------+
## fpga_v5a_q3
|          |          |      min      |      avg      |      max      |      min      |      avg      |      max      |                      |
+----------+----------+-----------------------------------------------+-----------------------------------------------+----------------------+
|      VHDL|        NA|             NA|             NA|             NA|             NA|             NA|             NA|                    NA|
|   Verilog|      Pass|           4345|           6265|           8629|           4367|           5105|           5843|                 18839|
+----------+----------+-----------------------------------------------+-----------------------------------------------+----------------------+
[cs2401.02@rce rce_upload]$ 