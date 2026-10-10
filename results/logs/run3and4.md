[cs2401.02@rce fpga_v4]$ for d in fpga_v4s0 fpga_v5; do echo "########## $d"; cd ~/$d; grep -E "^#define (RT|NT|LACC|RG|ACC_STYLE|USE_DATAFLOW) " src/kernel_cfg.h; echo "--- LOG"; grep -hE "HLS 200-887|HLS 200-871|HLS 200-1016|Final II" build/logs/csynth-*.log | cut -c1-400 | sort -u; for r in gemm tl_compute gv_compute load_b_tile tl_load gv_load gemv; do echo "--- REPORT $r"; sed -n '/== Performance Estimates/,/== Utilization Estimates/p' build/hls/hls/syn/report/${r}_csynth.rpt | cut -c1-220; sed -n '/== Utilization Estimates/,/+ Detail/p' build/hls/hls/syn/report/${r}_csynth.rpt; done; cd ~; done
########## fpga_v4s0
#define RT 4
#define NT 32
#define LACC 8
#define RG 8
#define ACC_STYLE 0
#define USE_DATAFLOW 0
--- LOG
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 13, loop 'VITIS_LOOP_219_6'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 1, loop 'VITIS_LOOP_68_6'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 2, loop 'VITIS_LOOP_142_6'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 2, loop 'VITIS_LOOP_77_1'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 3, loop 'VITIS_LOOP_158_1'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 3, loop 'VITIS_LOOP_318_1'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 3, loop 'VITIS_LOOP_88_1'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 42, loop 'VITIS_LOOP_49_2'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 4, loop 'VITIS_LOOP_22_1'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 75, loop 'VITIS_LOOP_128_3'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 76, loop 'VITIS_LOOP_324_2_VITIS_LOOP_325_3'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 77, loop 'VITIS_LOOP_117_1'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 80, loop 'VITIS_LOOP_134_4_VITIS_LOOP_135_5'
INFO: [HLS 200-1470] Pipelining result : Target II = NA, Final II = 8, Depth = 87, loop 'VITIS_LOOP_10_3'
Resolution: For help on HLS 200-1016 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-1016.html
Resolution: For help on HLS 200-871 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-871.html
Resolution: For help on HLS 200-887 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-887.html
WARNING: [HLS 200-1016] The critical path in module 'gv_compute_Pipeline_VITIS_LOOP_49_2' consists of the following:
WARNING: [HLS 200-1016] The critical path in module 'tl_compute_Pipeline_VITIS_LOOP_219_6' consists of the following:
WARNING: [HLS 200-871] Estimated clock period (2.729 ns) exceeds the target (target clock period: 3.330 ns, clock uncertainty: 0.899 ns, effective delay budget: 2.431 ns).
WARNING: [HLS 200-887] Cannot meet target clock period from 'fadd' operation 32 bit ('acc', ../src/kernel.cpp:65) to 'store' operation 0 bit ('acc_6_write_ln65', ../src/kernel.cpp:65) of variable 'acc', ../src/kernel.cpp:65 on local variable 'acc', ../src/kernel.cpp:38 (combination delay: 2.729 ns) to honor II or Latency constraint in region 'VITIS_LOOP_49_2'.
WARNING: [HLS 200-887] Cannot meet target clock period from 'fadd' operation 32 bit ('cur', ../src/kernel.cpp:255) to 'store' operation 0 bit ('empty_189_write_ln0') of variable 'add136_2255441_load' on local variable 'empty_189' (combination delay: 3.116 ns) to honor II or Latency constraint in region 'VITIS_LOOP_219_6'.
WARNING: [HLS 200-887] Cannot meet target clock period from 'store' operation 0 bit ('empty_151_write_ln0') of variable 'p_load552' on local variable 'empty_151' to 'fadd' operation 32 bit ('cur', ../src/kernel.cpp:255) (combination delay: 2.729 ns) to honor II or Latency constraint in region 'VITIS_LOOP_219_6'.
--- REPORT gemm
== Performance Estimates
================================================================
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  |  3.33 ns|  2.729 ns|     0.90 ns|
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +---------+---------+----------+----------+-----+-----+---------+
    |  Latency (cycles) |  Latency (absolute) |  Interval | Pipeline|
    |   min   |   max   |    min   |    max   | min | max |   Type  |
    +---------+---------+----------+----------+-----+-----+---------+
    |        ?|        ?|         ?|         ?|    ?|    ?|       no|
    +---------+---------+----------+----------+-----+-----+---------+

    + Detail: 
        * Instance: 
        +------------------------------------------------------------+-------------------------------------------------+---------+---------+----------+----------+-----+-----+----------------------------------------------
        |                                                            |                                                 |  Latency (cycles) |  Latency (absolute) |  Interval |                    Pipeline                  
        |                          Instance                          |                      Module                     |   min   |   max   |    min   |    max   | min | max |                      Type                    
        +------------------------------------------------------------+-------------------------------------------------+---------+---------+----------+----------+-----+-----+----------------------------------------------
        |grp_load_b_tile_fu_450                                      |load_b_tile                                      |        ?|        ?|         ?|         ?|    ?|    ?|                                              
        |grp_tl_load_fu_493                                          |tl_load                                          |        ?|        ?|         ?|         ?|    ?|    ?|                                              
        |grp_tl_compute_fu_507                                       |tl_compute                                       |        ?|        ?|         ?|         ?|    ?|    ?|                                              
        |grp_gemm_Pipeline_VITIS_LOOP_324_2_VITIS_LOOP_325_3_fu_548  |gemm_Pipeline_VITIS_LOOP_324_2_VITIS_LOOP_325_3  |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 clock cycles(s
        |grp_gemm_Pipeline_VITIS_LOOP_318_1_fu_689                   |gemm_Pipeline_VITIS_LOOP_318_1                   |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 clock cycles(s
        |grp_gemv_fu_826                                             |gemv                                             |        ?|        ?|         ?|         ?|    ?|    ?|                                              
        |grp_gemm_Pipeline_VITIS_LOOP_10_3_fu_841                    |gemm_Pipeline_VITIS_LOOP_10_3                    |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 clock cycles(s
        +------------------------------------------------------------+-------------------------------------------------+---------+---------+----------+----------+-----+-----+----------------------------------------------

        * Loop: 
        +---------------------+---------+---------+----------+-----------+-----------+---------------+----------+
        |                     |  Latency (cycles) | Iteration|  Initiation Interval  |      Trip     |          |
        |      Loop Name      |   min   |   max   |  Latency |  achieved |   target  |     Count     | Pipelined|
        +---------------------+---------+---------+----------+-----------+-----------+---------------+----------+
        |- VITIS_LOOP_7_1     |        ?|        ?|         ?|          -|          -|              ?|        no|
        | + VITIS_LOOP_8_2    |        ?|        ?|         ?|          -|          -|              ?|        no|
        |- VITIS_LOOP_338_1   |        ?|        ?|         ?|          -|          -|  1 ~ 134217728|        no|
        | + VITIS_LOOP_342_2  |        ?|        ?|         ?|          -|          -|          1 ~ ?|        no|
        +---------------------+---------+---------+----------+-----------+-----------+---------------+----------+



================================================================
== Utilization Estimates
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+--------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
+---------------------+---------+------+---------+--------+-----+
|DSP                  |        -|     -|        -|       -|    -|
|Expression           |        -|     -|        0|    1847|    -|
|FIFO                 |        -|     -|        -|       -|    -|
|Instance             |      103|   666|   189355|  113979|    0|
|Memory               |       96|     -|        0|       0|    0|
|Multiplexer          |        -|     -|        0|    4254|    -|
|Register             |        -|     -|     6228|       -|    -|
+---------------------+---------+------+---------+--------+-----+
|Total                |      199|   666|   195583|  120080|    0|
+---------------------+---------+------+---------+--------+-----+
|Available SLR        |     1344|  2976|   871680|  435840|  320|
+---------------------+---------+------+---------+--------+-----+
|Utilization SLR (%)  |       14|    22|       22|      27|    0|
+---------------------+---------+------+---------+--------+-----+
|Available            |     2688|  5952|  1743360|  871680|  640|
+---------------------+---------+------+---------+--------+-----+
|Utilization (%)      |        7|    11|       11|      13|    0|
+---------------------+---------+------+---------+--------+-----+

+ Detail: 
--- REPORT tl_compute
== Performance Estimates
================================================================
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  |  3.33 ns|  2.729 ns|     0.90 ns|
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +---------+---------+----------+----------+-----+-----+---------+
    |  Latency (cycles) |  Latency (absolute) |  Interval | Pipeline|
    |   min   |   max   |    min   |    max   | min | max |   Type  |
    +---------+---------+----------+----------+-----+-----+---------+
    |        ?|        ?|         ?|         ?|    ?|    ?|       no|
    +---------+---------+----------+----------+-----+-----+---------+

    + Detail: 
        * Instance: 
        +--------------------------------------------------+--------------------------------------+---------+---------+----------+----------+-----+-----+------------------------------------------------+
        |                                                  |                                      |  Latency (cycles) |  Latency (absolute) |  Interval |                    Pipeline                    |
        |                     Instance                     |                Module                |   min   |   max   |    min   |    max   | min | max |                      Type                      |
        +--------------------------------------------------+--------------------------------------+---------+---------+----------+----------+-----+-----+------------------------------------------------+
        |grp_tl_compute_Pipeline_VITIS_LOOP_219_6_fu_4186  |tl_compute_Pipeline_VITIS_LOOP_219_6  |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 clock cycles(s))|
        +--------------------------------------------------+--------------------------------------+---------+---------+----------+----------+-----+-----+------------------------------------------------+

        * Loop: 
        N/A



================================================================
== Utilization Estimates
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+--------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
+---------------------+---------+------+---------+--------+-----+
|DSP                  |        -|     -|        -|       -|    -|
|Expression           |        -|     -|        -|       -|    -|
|FIFO                 |        -|     -|        -|       -|    -|
|Instance             |        -|   560|   153309|   66395|    -|
|Memory               |        -|     -|        -|       -|    -|
|Multiplexer          |        -|     -|        0|   12153|    -|
|Register             |        -|     -|     8251|       -|    -|
+---------------------+---------+------+---------+--------+-----+
|Total                |        0|   560|   161560|   78548|    0|
+---------------------+---------+------+---------+--------+-----+
|Available SLR        |     1344|  2976|   871680|  435840|  320|
+---------------------+---------+------+---------+--------+-----+
|Utilization SLR (%)  |        0|    18|       18|      18|    0|
+---------------------+---------+------+---------+--------+-----+
|Available            |     2688|  5952|  1743360|  871680|  640|
+---------------------+---------+------+---------+--------+-----+
|Utilization (%)      |        0|     9|        9|       9|    0|
+---------------------+---------+------+---------+--------+-----+

+ Detail: 
--- REPORT gv_compute
== Performance Estimates
================================================================
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  |  3.33 ns|  2.729 ns|     0.90 ns|
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +---------+---------+----------+----------+-----+-----+---------+
    |  Latency (cycles) |  Latency (absolute) |  Interval | Pipeline|
    |   min   |   max   |    min   |    max   | min | max |   Type  |
    +---------+---------+----------+----------+-----+-----+---------+
    |        ?|        ?|         ?|         ?|    ?|    ?|       no|
    +---------+---------+----------+----------+-----+-----+---------+

    + Detail: 
        * Instance: 
        +------------------------------------------------+-------------------------------------+---------+---------+-----------+-----------+-----+-----+------------------------------------------------+
        |                                                |                                     |  Latency (cycles) |   Latency (absolute)  |  Interval |                    Pipeline                    |
        |                    Instance                    |                Module               |   min   |   max   |    min    |    max    | min | max |                      Type                      |
        +------------------------------------------------+-------------------------------------+---------+---------+-----------+-----------+-----+-----+------------------------------------------------+
        |grp_gv_compute_Pipeline_VITIS_LOOP_49_2_fu_88   |gv_compute_Pipeline_VITIS_LOOP_49_2  |        ?|        ?|          ?|          ?|    0|    0|  loop auto-rewind stp (delay=0 clock cycles(s))|
        |grp_gv_compute_Pipeline_VITIS_LOOP_68_6_fu_119  |gv_compute_Pipeline_VITIS_LOOP_68_6  |       10|       10|  33.300 ns|  33.300 ns|    9|    9|  loop auto-rewind stp (delay=0 clock cycles(s))|
        +------------------------------------------------+-------------------------------------+---------+---------+-----------+-----------+-----+-----+------------------------------------------------+

        * Loop: 
        N/A



================================================================
== Utilization Estimates
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+--------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
+---------------------+---------+------+---------+--------+-----+
|DSP                  |        -|     -|        -|       -|    -|
|Expression           |        -|     -|        -|       -|    -|
|FIFO                 |        -|     -|        -|       -|    -|
|Instance             |        -|     -|     2723|    1377|    -|
|Memory               |        -|     -|        -|       -|    -|
|Multiplexer          |        -|     -|        0|     386|    -|
|Register             |        -|     -|      290|       -|    -|
+---------------------+---------+------+---------+--------+-----+
|Total                |        0|     0|     3013|    1763|    0|
+---------------------+---------+------+---------+--------+-----+
|Available SLR        |     1344|  2976|   871680|  435840|  320|
+---------------------+---------+------+---------+--------+-----+
|Utilization SLR (%)  |        0|     0|       ~0|      ~0|    0|
+---------------------+---------+------+---------+--------+-----+
|Available            |     2688|  5952|  1743360|  871680|  640|
+---------------------+---------+------+---------+--------+-----+
|Utilization (%)      |        0|     0|       ~0|      ~0|    0|
+---------------------+---------+------+---------+--------+-----+

+ Detail: 
--- REPORT load_b_tile
== Performance Estimates
================================================================
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  |  3.33 ns|  2.431 ns|     0.90 ns|
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +---------+---------+----------+----------+-----+-----+---------+
    |  Latency (cycles) |  Latency (absolute) |  Interval | Pipeline|
    |   min   |   max   |    min   |    max   | min | max |   Type  |
    +---------+---------+----------+----------+-----+-----+---------+
    |        ?|        ?|         ?|         ?|    ?|    ?|       no|
    +---------+---------+----------+----------+-----+-----+---------+

    + Detail: 
        * Instance: 
        +-------------------------------------------------------------------+--------------------------------------------------------+---------+---------+----------+----------+-----+-----+--------------------------------
        |                                                                   |                                                        |  Latency (cycles) |  Latency (absolute) |  Interval |                    Pipeline    
        |                              Instance                             |                         Module                         |   min   |   max   |    min   |    max   | min | max |                      Type      
        +-------------------------------------------------------------------+--------------------------------------------------------+---------+---------+----------+----------+-----+-----+--------------------------------
        |grp_load_b_tile_Pipeline_VITIS_LOOP_134_4_VITIS_LOOP_135_5_fu_174  |load_b_tile_Pipeline_VITIS_LOOP_134_4_VITIS_LOOP_135_5  |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 
        |grp_load_b_tile_Pipeline_VITIS_LOOP_128_3_fu_249                   |load_b_tile_Pipeline_VITIS_LOOP_128_3                   |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 
        |grp_load_b_tile_Pipeline_VITIS_LOOP_117_1_fu_322                   |load_b_tile_Pipeline_VITIS_LOOP_117_1                   |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 
        |grp_load_b_tile_Pipeline_VITIS_LOOP_142_6_fu_396                   |load_b_tile_Pipeline_VITIS_LOOP_142_6                   |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 
        +-------------------------------------------------------------------+--------------------------------------------------------+---------+---------+----------+----------+-----+-----+--------------------------------

        * Loop: 
        N/A



================================================================
== Utilization Estimates
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+--------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
+---------------------+---------+------+---------+--------+-----+
|DSP                  |        -|     -|        -|       -|    -|
|Expression           |        -|     -|        0|     272|    -|
|FIFO                 |        -|     -|        -|       -|    -|
|Instance             |        -|     -|     3508|    7243|    -|
|Memory               |        -|     -|        -|       -|    -|
|Multiplexer          |        -|     -|        0|    3671|    -|
|Register             |        -|     -|      380|       -|    -|
+---------------------+---------+------+---------+--------+-----+
|Total                |        0|     0|     3888|   11186|    0|
+---------------------+---------+------+---------+--------+-----+
|Available SLR        |     1344|  2976|   871680|  435840|  320|
+---------------------+---------+------+---------+--------+-----+
|Utilization SLR (%)  |        0|     0|       ~0|       2|    0|
+---------------------+---------+------+---------+--------+-----+
|Available            |     2688|  5952|  1743360|  871680|  640|
+---------------------+---------+------+---------+--------+-----+
|Utilization (%)      |        0|     0|       ~0|       1|    0|
+---------------------+---------+------+---------+--------+-----+

+ Detail: 
--- REPORT tl_load
== Performance Estimates
================================================================
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  |  3.33 ns|  2.431 ns|     0.90 ns|
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +---------+---------+----------+----------+-----+-----+---------+
    |  Latency (cycles) |  Latency (absolute) |  Interval | Pipeline|
    |   min   |   max   |    min   |    max   | min | max |   Type  |
    +---------+---------+----------+----------+-----+-----+---------+
    |        ?|        ?|         ?|         ?|    ?|    ?|       no|
    +---------+---------+----------+----------+-----+-----+---------+

    + Detail: 
        * Instance: 
        +----------------------------------------------+-----------------------------------+---------+---------+----------+----------+-----+-----+------------------------------------------------+
        |                                              |                                   |  Latency (cycles) |  Latency (absolute) |  Interval |                    Pipeline                    |
        |                   Instance                   |               Module              |   min   |   max   |    min   |    max   | min | max |                      Type                      |
        +----------------------------------------------+-----------------------------------+---------+---------+----------+----------+-----+-----+------------------------------------------------+
        |grp_tl_load_Pipeline_VITIS_LOOP_158_1_fu_102  |tl_load_Pipeline_VITIS_LOOP_158_1  |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 clock cycles(s))|
        +----------------------------------------------+-----------------------------------+---------+---------+----------+----------+-----+-----+------------------------------------------------+

        * Loop: 
        N/A



================================================================
== Utilization Estimates
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+--------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
+---------------------+---------+------+---------+--------+-----+
|DSP                  |        -|     -|        -|       -|    -|
|Expression           |        -|     -|        0|     249|    -|
|FIFO                 |        -|     -|        -|       -|    -|
|Instance             |        -|     -|      680|     329|    -|
|Memory               |        -|     -|        -|       -|    -|
|Multiplexer          |        -|     -|        0|     505|    -|
|Register             |        -|     -|      320|       -|    -|
+---------------------+---------+------+---------+--------+-----+
|Total                |        0|     0|     1000|    1083|    0|
+---------------------+---------+------+---------+--------+-----+
|Available SLR        |     1344|  2976|   871680|  435840|  320|
+---------------------+---------+------+---------+--------+-----+
|Utilization SLR (%)  |        0|     0|       ~0|      ~0|    0|
+---------------------+---------+------+---------+--------+-----+
|Available            |     2688|  5952|  1743360|  871680|  640|
+---------------------+---------+------+---------+--------+-----+
|Utilization (%)      |        0|     0|       ~0|      ~0|    0|
+---------------------+---------+------+---------+--------+-----+

+ Detail: 
--- REPORT gv_load
== Performance Estimates
================================================================
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  |  3.33 ns|  2.431 ns|     0.90 ns|
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +---------+---------+----------+----------+-----+-----+---------+
    |  Latency (cycles) |  Latency (absolute) |  Interval | Pipeline|
    |   min   |   max   |    min   |    max   | min | max |   Type  |
    +---------+---------+----------+----------+-----+-----+---------+
    |        ?|        ?|         ?|         ?|    ?|    ?|       no|
    +---------+---------+----------+----------+-----+-----+---------+

    + Detail: 
        * Instance: 
        +---------------------------------------------+----------------------------------+---------+---------+----------+----------+-----+-----+------------------------------------------------+
        |                                             |                                  |  Latency (cycles) |  Latency (absolute) |  Interval |                    Pipeline                    |
        |                   Instance                  |              Module              |   min   |   max   |    min   |    max   | min | max |                      Type                      |
        +---------------------------------------------+----------------------------------+---------+---------+----------+----------+-----+-----+------------------------------------------------+
        |grp_gv_load_Pipeline_VITIS_LOOP_22_1_fu_112  |gv_load_Pipeline_VITIS_LOOP_22_1  |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 clock cycles(s))|
        +---------------------------------------------+----------------------------------+---------+---------+----------+----------+-----+-----+------------------------------------------------+

        * Loop: 
        N/A



================================================================
== Utilization Estimates
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+--------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
+---------------------+---------+------+---------+--------+-----+
|DSP                  |        -|     -|        -|       -|    -|
|Expression           |        -|     -|        0|     248|    -|
|FIFO                 |        -|     -|        -|       -|    -|
|Instance             |        -|     4|     1453|     558|    -|
|Memory               |        -|     -|        -|       -|    -|
|Multiplexer          |        -|     -|        0|     505|    -|
|Register             |        -|     -|      320|       -|    -|
+---------------------+---------+------+---------+--------+-----+
|Total                |        0|     4|     1773|    1311|    0|
+---------------------+---------+------+---------+--------+-----+
|Available SLR        |     1344|  2976|   871680|  435840|  320|
+---------------------+---------+------+---------+--------+-----+
|Utilization SLR (%)  |        0|    ~0|       ~0|      ~0|    0|
+---------------------+---------+------+---------+--------+-----+
|Available            |     2688|  5952|  1743360|  871680|  640|
+---------------------+---------+------+---------+--------+-----+
|Utilization (%)      |        0|    ~0|       ~0|      ~0|    0|
+---------------------+---------+------+---------+--------+-----+

+ Detail: 
--- REPORT gemv
== Performance Estimates
================================================================
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  |  3.33 ns|  2.729 ns|     0.90 ns|
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +---------+---------+----------+----------+-----+-----+---------+
    |  Latency (cycles) |  Latency (absolute) |  Interval | Pipeline|
    |   min   |   max   |    min   |    max   | min | max |   Type  |
    +---------+---------+----------+----------+-----+-----+---------+
    |        ?|        ?|         ?|         ?|    ?|    ?|       no|
    +---------+---------+----------+----------+-----+-----+---------+

    + Detail: 
        * Instance: 
        +------------------------------------------+-------------------------------+---------+-----------+----------+-----------+-----+-----------+------------------------------------------------+
        |                                          |                               |   Latency (cycles)  |  Latency (absolute)  |     Interval    |                    Pipeline                    |
        |                 Instance                 |             Module            |   min   |    max    |    min   |    max    | min |    max    |                      Type                      |
        +------------------------------------------+-------------------------------+---------+-----------+----------+-----------+-----+-----------+------------------------------------------------+
        |grp_gemv_Pipeline_VITIS_LOOP_88_1_fu_235  |gemv_Pipeline_VITIS_LOOP_88_1  |        2|  134217730|  6.660 ns|  0.447 sec|    1|  134217728|  loop auto-rewind stp (delay=0 clock cycles(s))|
        |grp_gv_load_fu_244                        |gv_load                        |        ?|          ?|         ?|          ?|    ?|          ?|                                              no|
        |grp_gv_compute_fu_262                     |gv_compute                     |        ?|          ?|         ?|          ?|    ?|          ?|                                              no|
        |grp_gemv_Pipeline_VITIS_LOOP_77_1_fu_284  |gemv_Pipeline_VITIS_LOOP_77_1  |        2|         10|  6.660 ns|  33.300 ns|    1|          9|  loop auto-rewind stp (delay=0 clock cycles(s))|
        +------------------------------------------+-------------------------------+---------+-----------+----------+-----------+-----+-----------+------------------------------------------------+

        * Loop: 
        +-------------------+---------+---------+----------+-----------+-----------+---------------+----------+
        |                   |  Latency (cycles) | Iteration|  Initiation Interval  |      Trip     |          |
        |     Loop Name     |   min   |   max   |  Latency |  achieved |   target  |     Count     | Pipelined|
        +-------------------+---------+---------+----------+-----------+-----------+---------------+----------+
        |- VITIS_LOOP_96_3  |        ?|        ?|         ?|          -|          -|  1 ~ 536870912|        no|
        +-------------------+---------+---------+----------+-----------+-----------+---------------+----------+



================================================================
== Utilization Estimates
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+--------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
+---------------------+---------+------+---------+--------+-----+
|DSP                  |        -|     -|        -|       -|    -|
|Expression           |        -|     -|        0|     362|    -|
|FIFO                 |        -|     -|        -|       -|    -|
|Instance             |        -|     4|     5361|    3314|    -|
|Memory               |       72|     -|        0|       0|    0|
|Multiplexer          |        -|     -|        0|    1879|    -|
|Register             |        -|     -|      749|       -|    -|
+---------------------+---------+------+---------+--------+-----+
|Total                |       72|     4|     6110|    5555|    0|
+---------------------+---------+------+---------+--------+-----+
|Available SLR        |     1344|  2976|   871680|  435840|  320|
+---------------------+---------+------+---------+--------+-----+
|Utilization SLR (%)  |        5|    ~0|       ~0|       1|    0|
+---------------------+---------+------+---------+--------+-----+
|Available            |     2688|  5952|  1743360|  871680|  640|
+---------------------+---------+------+---------+--------+-----+
|Utilization (%)      |        2|    ~0|       ~0|      ~0|    0|
+---------------------+---------+------+---------+--------+-----+

+ Detail: 
########## fpga_v5
#define RT 8
#define NT 32
#define LACC 8
#define RG 16
#define ACC_STYLE 2
#define USE_DATAFLOW 1
--- LOG
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 13, loop 'VITIS_LOOP_219_6'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 1, loop 'VITIS_LOOP_188_1'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 1, loop 'VITIS_LOOP_44_1'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 2, loop 'VITIS_LOOP_142_6'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 2, loop 'VITIS_LOOP_268_14'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 2, loop 'VITIS_LOOP_68_6'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 2, loop 'VITIS_LOOP_77_1'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 3, loop 'VITIS_LOOP_318_1'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 3, loop 'VITIS_LOOP_88_1'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 43, loop 'VITIS_LOOP_49_2'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 4, loop 'VITIS_LOOP_158_1'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 75, loop 'VITIS_LOOP_128_3'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 76, loop 'VITIS_LOOP_324_2_VITIS_LOOP_325_3'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 77, loop 'VITIS_LOOP_117_1'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 80, loop 'VITIS_LOOP_134_4_VITIS_LOOP_135_5'
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 8, Depth = 19, loop 'VITIS_LOOP_22_1'
INFO: [HLS 200-1470] Pipelining result : Target II = NA, Final II = 8, Depth = 87, loop 'VITIS_LOOP_10_3'
Resolution: For help on HLS 200-1016 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-1016.html
Resolution: For help on HLS 200-871 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-871.html
Resolution: For help on HLS 200-887 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-887.html
WARNING: [HLS 200-1016] The critical path in module 'tl_compute8_Pipeline_VITIS_LOOP_219_6' consists of the following:
WARNING: [HLS 200-871] Estimated clock period (3.041 ns) exceeds the target (target clock period: 3.330 ns, clock uncertainty: 0.899 ns, effective delay budget: 2.431 ns).
WARNING: [HLS 200-887] Cannot meet target clock period from 'fadd' operation 32 bit ('add137_7_18', ../src/kernel.cpp:253) to 'store' operation 0 bit ('sh_243_addr_write_ln253', ../src/kernel.cpp:253) of variable 'add137_7_18', ../src/kernel.cpp:253 on array 'sh_243' (combination delay: 3.041 ns) to honor II or Latency constraint in region 'VITIS_LOOP_219_6'.
--- REPORT gemm
== Performance Estimates
================================================================
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  |  3.33 ns|  3.491 ns|     0.90 ns|
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +---------+---------+----------+----------+-----+-----+---------+
    |  Latency (cycles) |  Latency (absolute) |  Interval | Pipeline|
    |   min   |   max   |    min   |    max   | min | max |   Type  |
    +---------+---------+----------+----------+-----+-----+---------+
    |        ?|        ?|         ?|         ?|    ?|    ?|       no|
    +---------+---------+----------+----------+-----+-----+---------+

    + Detail: 
        * Instance: 
        +------------------------------------------+-------------------------------+---------+---------+----------+----------+-----+-----+------------------------------------------------+
        |                                          |                               |  Latency (cycles) |  Latency (absolute) |  Interval |                    Pipeline                    |
        |                 Instance                 |             Module            |   min   |   max   |    min   |    max   | min | max |                      Type                      |
        +------------------------------------------+-------------------------------+---------+---------+----------+----------+-----+-----+------------------------------------------------+
        |grp_load_b_tile_fu_369                    |load_b_tile                    |        ?|        ?|         ?|         ?|    ?|    ?|                                              no|
        |grp_dataflow_parent_loop_proc3_fu_412     |dataflow_parent_loop_proc3     |        ?|        ?|         ?|         ?|    ?|    ?|                                              no|
        |grp_gemv_fu_458                           |gemv                           |        ?|        ?|         ?|         ?|    ?|    ?|                                              no|
        |grp_gemm_Pipeline_VITIS_LOOP_10_3_fu_473  |gemm_Pipeline_VITIS_LOOP_10_3  |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 clock cycles(s))|
        +------------------------------------------+-------------------------------+---------+---------+----------+----------+-----+-----+------------------------------------------------+

        * Loop: 
        +--------------------+---------+---------+----------+-----------+-----------+---------------+----------+
        |                    |  Latency (cycles) | Iteration|  Initiation Interval  |      Trip     |          |
        |      Loop Name     |   min   |   max   |  Latency |  achieved |   target  |     Count     | Pipelined|
        +--------------------+---------+---------+----------+-----------+-----------+---------------+----------+
        |- VITIS_LOOP_7_1    |        ?|        ?|         ?|          -|          -|              ?|        no|
        | + VITIS_LOOP_8_2   |        ?|        ?|         ?|          -|          -|              ?|        no|
        |- VITIS_LOOP_338_1  |        ?|        ?|         ?|          -|          -|  1 ~ 134217728|        no|
        +--------------------+---------+---------+----------+-----------+-----------+---------------+----------+



================================================================
== Utilization Estimates
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+--------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
+---------------------+---------+------+---------+--------+-----+
|DSP                  |        -|     -|        -|       -|    -|
|Expression           |        -|     -|        0|     913|    -|
|FIFO                 |        -|     -|        -|       -|    -|
|Instance             |      423|  2947|   557979|  340553|    0|
|Memory               |       64|     -|        0|       0|    0|
|Multiplexer          |        -|     -|        0|    1972|    -|
|Register             |        -|     -|     1009|       -|    -|
+---------------------+---------+------+---------+--------+-----+
|Total                |      487|  2947|   558988|  343438|    0|
+---------------------+---------+------+---------+--------+-----+
|Available SLR        |     1344|  2976|   871680|  435840|  320|
+---------------------+---------+------+---------+--------+-----+
|Utilization SLR (%)  |       36|    99|       64|      78|    0|
+---------------------+---------+------+---------+--------+-----+
|Available            |     2688|  5952|  1743360|  871680|  640|
+---------------------+---------+------+---------+--------+-----+
|Utilization (%)      |       18|    49|       32|      39|    0|
+---------------------+---------+------+---------+--------+-----+

+ Detail: 
--- REPORT tl_compute
sed: can't read build/hls/hls/syn/report/tl_compute_csynth.rpt: No such file or directory
sed: can't read build/hls/hls/syn/report/tl_compute_csynth.rpt: No such file or directory
--- REPORT gv_compute
sed: can't read build/hls/hls/syn/report/gv_compute_csynth.rpt: No such file or directory
sed: can't read build/hls/hls/syn/report/gv_compute_csynth.rpt: No such file or directory
--- REPORT load_b_tile
== Performance Estimates
================================================================
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  |  3.33 ns|  2.431 ns|     0.90 ns|
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +---------+---------+----------+----------+-----+-----+---------+
    |  Latency (cycles) |  Latency (absolute) |  Interval | Pipeline|
    |   min   |   max   |    min   |    max   | min | max |   Type  |
    +---------+---------+----------+----------+-----+-----+---------+
    |        ?|        ?|         ?|         ?|    ?|    ?|       no|
    +---------+---------+----------+----------+-----+-----+---------+

    + Detail: 
        * Instance: 
        +-------------------------------------------------------------------+--------------------------------------------------------+---------+---------+----------+----------+-----+-----+--------------------------------
        |                                                                   |                                                        |  Latency (cycles) |  Latency (absolute) |  Interval |                    Pipeline    
        |                              Instance                             |                         Module                         |   min   |   max   |    min   |    max   | min | max |                      Type      
        +-------------------------------------------------------------------+--------------------------------------------------------+---------+---------+----------+----------+-----+-----+--------------------------------
        |grp_load_b_tile_Pipeline_VITIS_LOOP_134_4_VITIS_LOOP_135_5_fu_176  |load_b_tile_Pipeline_VITIS_LOOP_134_4_VITIS_LOOP_135_5  |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 
        |grp_load_b_tile_Pipeline_VITIS_LOOP_128_3_fu_251                   |load_b_tile_Pipeline_VITIS_LOOP_128_3                   |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 
        |grp_load_b_tile_Pipeline_VITIS_LOOP_117_1_fu_324                   |load_b_tile_Pipeline_VITIS_LOOP_117_1                   |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 
        |grp_load_b_tile_Pipeline_VITIS_LOOP_142_6_fu_398                   |load_b_tile_Pipeline_VITIS_LOOP_142_6                   |        ?|        ?|         ?|         ?|    0|    0|  loop auto-rewind stp (delay=0 
        +-------------------------------------------------------------------+--------------------------------------------------------+---------+---------+----------+----------+-----+-----+--------------------------------

        * Loop: 
        N/A



================================================================
== Utilization Estimates
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+--------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
+---------------------+---------+------+---------+--------+-----+
|DSP                  |        -|     -|        -|       -|    -|
|Expression           |        -|     -|        0|     247|    -|
|FIFO                 |        -|     -|        -|       -|    -|
|Instance             |        -|    13|     4180|    7406|    -|
|Memory               |        -|     -|        -|       -|    -|
|Multiplexer          |        -|     -|        0|    3653|    -|
|Register             |        -|     -|      329|       -|    -|
+---------------------+---------+------+---------+--------+-----+
|Total                |        0|    13|     4509|   11306|    0|
+---------------------+---------+------+---------+--------+-----+
|Available SLR        |     1344|  2976|   871680|  435840|  320|
+---------------------+---------+------+---------+--------+-----+
|Utilization SLR (%)  |        0|    ~0|       ~0|       2|    0|
+---------------------+---------+------+---------+--------+-----+
|Available            |     2688|  5952|  1743360|  871680|  640|
+---------------------+---------+------+---------+--------+-----+
|Utilization (%)      |        0|    ~0|       ~0|       1|    0|
+---------------------+---------+------+---------+--------+-----+

+ Detail: 
--- REPORT tl_load
sed: can't read build/hls/hls/syn/report/tl_load_csynth.rpt: No such file or directory
sed: can't read build/hls/hls/syn/report/tl_load_csynth.rpt: No such file or directory
--- REPORT gv_load
sed: can't read build/hls/hls/syn/report/gv_load_csynth.rpt: No such file or directory
sed: can't read build/hls/hls/syn/report/gv_load_csynth.rpt: No such file or directory
--- REPORT gemv
== Performance Estimates
================================================================
+ Timing: 
    * Summary: 
    +--------+---------+----------+------------+
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  |  3.33 ns|  3.491 ns|     0.90 ns|
    +--------+---------+----------+------------+

+ Latency: 
    * Summary: 
    +---------+---------+----------+----------+-----+-----+---------+
    |  Latency (cycles) |  Latency (absolute) |  Interval | Pipeline|
    |   min   |   max   |    min   |    max   | min | max |   Type  |
    +---------+---------+----------+----------+-----+-----+---------+
    |        ?|        ?|         ?|         ?|    ?|    ?|       no|
    +---------+---------+----------+----------+-----+-----+---------+

    + Detail: 
        * Instance: 
        +------------------------------------------+-------------------------------+---------+-----------+----------+-----------+-----+-----------+------------------------------------------------+
        |                                          |                               |   Latency (cycles)  |  Latency (absolute)  |     Interval    |                    Pipeline                    |
        |                 Instance                 |             Module            |   min   |    max    |    min   |    max    | min |    max    |                      Type                      |
        +------------------------------------------+-------------------------------+---------+-----------+----------+-----------+-----+-----------+------------------------------------------------+
        |grp_gemv_Pipeline_VITIS_LOOP_88_1_fu_116  |gemv_Pipeline_VITIS_LOOP_88_1  |        2|  134217730|  6.660 ns|  0.447 sec|    1|  134217728|  loop auto-rewind stp (delay=0 clock cycles(s))|
        |grp_dataflow_parent_loop_proc_fu_125      |dataflow_parent_loop_proc      |        ?|          ?|         ?|          ?|    ?|          ?|                                              no|
        +------------------------------------------+-------------------------------+---------+-----------+----------+-----------+-----+-----------+------------------------------------------------+

        * Loop: 
        N/A



================================================================
== Utilization Estimates
== Utilization Estimates
================================================================
* Summary: 
+---------------------+---------+------+---------+--------+-----+
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
+---------------------+---------+------+---------+--------+-----+
|DSP                  |        -|     -|        -|       -|    -|
|Expression           |        -|     -|        0|      64|    -|
|FIFO                 |        -|     -|        -|       -|    -|
|Instance             |      256|    87|    15569|   12858|    0|
|Memory               |        8|     -|        0|       0|    0|
|Multiplexer          |        -|     -|        0|     535|    -|
|Register             |        -|     -|      193|       -|    -|
+---------------------+---------+------+---------+--------+-----+
|Total                |      264|    87|    15762|   13457|    0|
+---------------------+---------+------+---------+--------+-----+
|Available SLR        |     1344|  2976|   871680|  435840|  320|
+---------------------+---------+------+---------+--------+-----+
|Utilization SLR (%)  |       19|     2|        1|       3|    0|
+---------------------+---------+------+---------+--------+-----+
|Available            |     2688|  5952|  1743360|  871680|  640|
+---------------------+---------+------+---------+--------+-----+
|Utilization (%)      |        9|     1|       ~0|       1|    0|
+---------------------+---------+------+---------+--------+-----+

+ Detail: 
[cs2401.02@rce ~]$ 