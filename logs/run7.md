[cs2401.02@rce rce_upload]$ cd ~/rce_upload && ./report.sh fpga_v5b_frp fpga_v12_frp fpga_v6_frp
########## fpga_v5b_frp
#define RT 8 #define NT 32 #define LACC 8 #define RG 8 #define ACC_STYLE 0 #define USE_DATAFLOW 1 
2
Resolution: For help on HLS 200-871 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug144
WARNING: [HLS 200-871] Estimated clock period (2.729 ns) exceeds the target (target clock period: 3.330 ns, clock uncert
        'fadd' operation 32 bit ('acc', ../src/kernel.cpp:65->../src/kernel.cpp:106->../src/kernel.cpp:103) [429]  (2.342 ns)
        'fadd' operation 32 bit ('cur', ../src/kernel.cpp:255) [9628]  (2.342 ns)
        'store' operation 0 bit ('acc_7_write_ln65', ../src/kernel.cpp:65->../src/kernel.cpp:106->../src/kernel.cpp:103) of variable 'acc', ../src/kernel.cpp:65->../src/kernel.c
        'store' operation 0 bit ('sh_2040_write_ln255', ../src/kernel.cpp:255) of variable 'cur', ../src/kernel.cpp:255 on local variable 'sh_2040' [9629]  (0.387 ns)
WARNING: [HLS 200-1016] The critical path in module 'gv_compute5_Pipeline_VITIS_LOOP_49_2' consists of the following:
WARNING: [HLS 200-1016] The critical path in module 'tl_compute8_Pipeline_VITIS_LOOP_219_6' consists of the following:
|Total                |      359|  1411|   386549|  214197|    0|
|Utilization SLR (%)  |       26|    47|       44|      49|    0|
    |ap_clk  |  3.33 ns|  3.491 ns|     0.90 ns|
########## fpga_v12_frp
#define RT 12 #define NT 32 #define LACC 8 #define RG 8 #define ACC_STYLE 0 #define USE_DATAFLOW 1 
2
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 8, Depth = 19, loop 'VITIS_LOOP_158_1'
Resolution: For help on HLS 200-871 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug144
WARNING: [HLS 200-871] Estimated clock period (2.729 ns) exceeds the target (target clock period: 3.330 ns, clock uncert
        'fadd' operation 32 bit ('acc', ../src/kernel.cpp:65->../src/kernel.cpp:106->../src/kernel.cpp:103) [429]  (2.342 ns)
        'fadd' operation 32 bit ('cur', ../src/kernel.cpp:255) [14368]  (2.342 ns)
        'store' operation 0 bit ('acc_7_write_ln65', ../src/kernel.cpp:65->../src/kernel.cpp:106->../src/kernel.cpp:103) of variable 'acc', ../src/kernel.cpp:65->../src/kernel.c
        'store' operation 0 bit ('sh_3064_write_ln255', ../src/kernel.cpp:255) of variable 'cur', ../src/kernel.cpp:255 on local variable 'sh_3064' [14369]  (0.387 ns)
WARNING: [HLS 200-1016] The critical path in module 'gv_compute5_Pipeline_VITIS_LOOP_49_2' consists of the following:
WARNING: [HLS 200-1016] The critical path in module 'tl_compute8_Pipeline_VITIS_LOOP_219_6' consists of the following:
|Total                |      423|  2051|   563254|  304264|    0|
|Utilization SLR (%)  |       31|    68|       64|      69|    0|
    |ap_clk  |  3.33 ns|  3.491 ns|     0.90 ns|
########## fpga_v6_frp
#define RT 16 #define NT 32 #define LACC 8 #define RG 8 #define ACC_STYLE 0 #define USE_DATAFLOW 1 
2
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 8, Depth = 19, loop 'VITIS_LOOP_158_1'
Resolution: For help on HLS 200-871 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug144
WARNING: [HLS 200-871] Estimated clock period (2.729 ns) exceeds the target (target clock period: 3.330 ns, clock uncert
        'fadd' operation 32 bit ('acc', ../src/kernel.cpp:65->../src/kernel.cpp:106->../src/kernel.cpp:103) [429]  (2.342 ns)
        'fadd' operation 32 bit ('cur', ../src/kernel.cpp:255) [19108]  (2.342 ns)
        'store' operation 0 bit ('acc_7_write_ln65', ../src/kernel.cpp:65->../src/kernel.cpp:106->../src/kernel.cpp:103) of variable 'acc', ../src/kernel.cpp:65->../src/kernel.c
        'store' operation 0 bit ('sh_4088_write_ln174', ../src/kernel.cpp:174) of variable 'cur', ../src/kernel.cpp:255 on local variable 'sh', ../src/kernel.cpp:174 [19112]  (0
WARNING: [HLS 200-1016] The critical path in module 'gv_compute5_Pipeline_VITIS_LOOP_49_2' consists of the following:
WARNING: [HLS 200-1016] The critical path in module 'tl_compute8_Pipeline_VITIS_LOOP_219_6' consists of the following:
|Total                |      487|  2691|   608767|  359845|    0|
|Utilization SLR (%)  |       36|    90|       69|      82|    0|
    |ap_clk  |  3.33 ns|  3.491 ns|     0.90 ns|
[cs2401.02@rce rce_upload]$ for d in fpga_v5a_p fpga_v5b_p; do echo "## $d"; sed -n 8,14p ~/$d/build/hls/hls/sim/report/gemm_cosim.rpt; done
## fpga_v5a_p
|          |          |      min      |      avg      |      max      |      min      |      avg      |      max      |                      |
+----------+----------+-----------------------------------------------+-----------------------------------------------+----------------------+
|      VHDL|        NA|             NA|             NA|             NA|             NA|             NA|             NA|                    NA|
|   Verilog|      Pass|            828|          11971|          52117|            853|          14448|          52142|                 59960|
+----------+----------+-----------------------------------------------+-----------------------------------------------+----------------------+
## fpga_v5b_p
|          |          |      min      |      avg      |      max      |      min      |      avg      |      max      |                      |
+----------+----------+-----------------------------------------------+-----------------------------------------------+----------------------+
|      VHDL|        NA|             NA|             NA|             NA|             NA|             NA|             NA|                    NA|
|   Verilog|      Pass|            828|          11940|          52119|            853|          14410|          52144|                 59806|
+----------+----------+-----------------------------------------------+-----------------------------------------------+----------------------+
[cs2401.02@rce rce_upload]$ 