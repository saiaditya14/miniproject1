[cs2401.02@rce fpga_v5bcf]$ cd ~/rce_upload/fpga_s3/build/hls/hls/syn/report; ls | head -30; grep -A3 "Estimated" gemm_csynth.rpt | head -5; for f in *gv_*_csynth.rpt; do echo "## $f"; grep -E "Estimated|VITIS_LOOP|BRAM|DSP" $f | cut -c1-150 | head -14; done
csynth_design_size.rpt
csynth_design_size.xml
csynth.rpt
csynth.xml
dataflow_in_loop_VITIS_LOOP_220_1_1_Block_newFuncRoot_proc_csynth.rpt
dataflow_in_loop_VITIS_LOOP_220_1_1_Block_newFuncRoot_proc_csynth.xml
dataflow_in_loop_VITIS_LOOP_220_1_1_csynth.rpt
dataflow_in_loop_VITIS_LOOP_220_1_1_csynth.xml
dataflow_in_loop_VITIS_LOOP_510_2_1_csynth.rpt
dataflow_in_loop_VITIS_LOOP_510_2_1_csynth.xml
dataflow_parent_loop_proc_csynth.rpt
dataflow_parent_loop_proc_csynth.xml
gemm_csynth.rpt
gemm_csynth.xml
gemm_Pipeline_VITIS_LOOP_13_3_csynth.rpt
gemm_Pipeline_VITIS_LOOP_13_3_csynth.xml
gemm_Pipeline_VITIS_LOOP_263_1_csynth.rpt
gemm_Pipeline_VITIS_LOOP_263_1_csynth.xml
gv_compute_csynth.rpt
gv_compute_csynth.xml
gv_compute_Pipeline_VITIS_LOOP_167_1_csynth.rpt
gv_compute_Pipeline_VITIS_LOOP_167_1_csynth.xml
gv_engine_csynth.rpt
gv_engine_csynth.xml
gv_fill_csynth.rpt
gv_fill_csynth.xml
gv_fill_Pipeline_VITIS_LOOP_133_1_csynth.rpt
gv_fill_Pipeline_VITIS_LOOP_133_1_csynth.xml
gv_reader_csynth.rpt
gv_reader_csynth.xml
    |  Clock |  Target | Estimated| Uncertainty|
    +--------+---------+----------+------------+
    |ap_clk  |  3.33 ns|  3.491 ns|     0.90 ns|
    +--------+---------+----------+------------+
## gv_compute_csynth.rpt
    |  Clock |  Target | Estimated| Uncertainty|
        |grp_gv_compute_Pipeline_VITIS_LOOP_167_1_fu_86  |gv_compute_Pipeline_VITIS_LOOP_167_1  |        ?|        ?|         ?|         ?|    0|    0
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
|DSP                  |        -|     -|        -|       -|    -|
    |                    Instance                    |                Module                | BRAM_18K| DSP|   FF  |  LUT | URAM|
    |grp_gv_compute_Pipeline_VITIS_LOOP_167_1_fu_86  |gv_compute_Pipeline_VITIS_LOOP_167_1  |        0|  80|  10417|  5144|    0|
    * DSP: 
    |     Variable Name     | Operation| DSP| FF| LUT| Bitwidth P0| Bitwidth P1|
    |grp_gv_compute_Pipeline_VITIS_LOOP_167_1_fu_86_ap_start_reg  |   1|   0|    1|          0|
## gv_compute_Pipeline_VITIS_LOOP_167_1_csynth.rpt
== Vitis HLS Report for 'gv_compute_Pipeline_VITIS_LOOP_167_1'
    |  Clock |  Target | Estimated| Uncertainty|
        |- VITIS_LOOP_167_1  |        ?|        ?|        43|          1|          1|     ?|       yes|
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
|DSP                  |        -|     -|        -|       -|    -|
    |               Instance               |             Module             | BRAM_18K| DSP|  FF | LUT | URAM|
    * DSP: 
    |     Variable Name    | Operation| DSP| FF| LUT| Bitwidth P0| Bitwidth P1|
|ap_clk          |   in|    1|  ap_ctrl_hs|  gv_compute_Pipeline_VITIS_LOOP_167_1|  return value|
|ap_rst          |   in|    1|  ap_ctrl_hs|  gv_compute_Pipeline_VITIS_LOOP_167_1|  return value|
|ap_start        |   in|    1|  ap_ctrl_hs|  gv_compute_Pipeline_VITIS_LOOP_167_1|  return value|
|ap_done         |  out|    1|  ap_ctrl_hs|  gv_compute_Pipeline_VITIS_LOOP_167_1|  return value|
|ap_idle         |  out|    1|  ap_ctrl_hs|  gv_compute_Pipeline_VITIS_LOOP_167_1|  return value|
|ap_ready        |  out|    1|  ap_ctrl_hs|  gv_compute_Pipeline_VITIS_LOOP_167_1|  return value|
## gv_engine_csynth.rpt
    |  Clock |  Target | Estimated| Uncertainty|
        |dataflow_in_loop_VITIS_LOOP_220_1_1_U0  |dataflow_in_loop_VITIS_LOOP_220_1_1  |        ?|        ?|         ?|         ?|    ?|    ?|  datafl
        |- VITIS_LOOP_220_1  |        ?|        ?|         ?|          -|          -|  0 ~ 33554432|        no|
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
|DSP                  |        -|     -|        -|       -|    -|
    |                Instance                |                Module               | BRAM_18K| DSP|   FF  |  LUT | URAM|
    |dataflow_in_loop_VITIS_LOOP_220_1_1_U0  |dataflow_in_loop_VITIS_LOOP_220_1_1  |      512|  86|  12126|  7063|    0|
    * DSP: 
    |       Variable Name       | Operation| DSP| FF| LUT| Bitwidth P0| Bitwidth P1|
## gv_fill_csynth.rpt
    |  Clock |  Target | Estimated| Uncertainty|
        |grp_gv_fill_Pipeline_VITIS_LOOP_133_1_fu_102  |gv_fill_Pipeline_VITIS_LOOP_133_1  |        ?|        ?|         ?|         ?|    0|    0|  lo
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
|DSP                  |        -|     -|        -|       -|    -|
    |                   Instance                   |               Module              | BRAM_18K| DSP|  FF | LUT | URAM|
    |grp_gv_fill_Pipeline_VITIS_LOOP_133_1_fu_102  |gv_fill_Pipeline_VITIS_LOOP_133_1  |        0|   0|  627|  293|    0|
    * DSP: 
    |  Variable Name  | Operation| DSP| FF| LUT| Bitwidth P0| Bitwidth P1|
    |grp_gv_fill_Pipeline_VITIS_LOOP_133_1_fu_102_ap_start_reg  |   1|   0|    1|          0|
## gv_fill_Pipeline_VITIS_LOOP_133_1_csynth.rpt
== Vitis HLS Report for 'gv_fill_Pipeline_VITIS_LOOP_133_1'
    |  Clock |  Target | Estimated| Uncertainty|
        |- VITIS_LOOP_133_1  |        ?|        ?|         3|          1|          1|     ?|       yes|
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
|DSP                  |        -|     -|        -|       -|    -|
    * DSP: 
    |          Variable Name          | Operation| DSP| FF| LUT| Bitwidth P0| Bitwidth P1|
|ap_clk                |   in|    1|  ap_ctrl_hs|  gv_fill_Pipeline_VITIS_LOOP_133_1|  return value|
|ap_rst                |   in|    1|  ap_ctrl_hs|  gv_fill_Pipeline_VITIS_LOOP_133_1|  return value|
|ap_start              |   in|    1|  ap_ctrl_hs|  gv_fill_Pipeline_VITIS_LOOP_133_1|  return value|
|ap_done               |  out|    1|  ap_ctrl_hs|  gv_fill_Pipeline_VITIS_LOOP_133_1|  return value|
|ap_idle               |  out|    1|  ap_ctrl_hs|  gv_fill_Pipeline_VITIS_LOOP_133_1|  return value|
|ap_ready              |  out|    1|  ap_ctrl_hs|  gv_fill_Pipeline_VITIS_LOOP_133_1|  return value|
## gv_reader_csynth.rpt
    |  Clock |  Target | Estimated| Uncertainty|
        |grp_gv_reader_Pipeline_VITIS_LOOP_122_1_fu_100  |gv_reader_Pipeline_VITIS_LOOP_122_1  |        ?|        ?|         ?|         ?|    0|    0|
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
|DSP                  |        -|     -|        -|       -|    -|
    |                    Instance                    |                Module               | BRAM_18K| DSP|  FF | LUT | URAM|
    |grp_gv_reader_Pipeline_VITIS_LOOP_122_1_fu_100  |gv_reader_Pipeline_VITIS_LOOP_122_1  |        0|   0|  548|  272|    0|
    * DSP: 
    |     Variable Name    | Operation| DSP| FF| LUT| Bitwidth P0| Bitwidth P1|
    |grp_gv_reader_Pipeline_VITIS_LOOP_122_1_fu_100_ap_start_reg  |   1|   0|    1|          0|
## gv_reader_Pipeline_VITIS_LOOP_122_1_csynth.rpt
== Vitis HLS Report for 'gv_reader_Pipeline_VITIS_LOOP_122_1'
    |  Clock |  Target | Estimated| Uncertainty|
        |- VITIS_LOOP_122_1  |        ?|        ?|         3|          1|          1|     ?|       yes|
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
|DSP                  |        -|     -|        -|       -|    -|
    * DSP: 
    |             Variable Name             | Operation| DSP| FF| LUT| Bitwidth P0| Bitwidth P1|
|ap_clk                  |   in|    1|  ap_ctrl_hs|  gv_reader_Pipeline_VITIS_LOOP_122_1|  return value|
|ap_rst                  |   in|    1|  ap_ctrl_hs|  gv_reader_Pipeline_VITIS_LOOP_122_1|  return value|
|ap_start                |   in|    1|  ap_ctrl_hs|  gv_reader_Pipeline_VITIS_LOOP_122_1|  return value|
|ap_done                 |  out|    1|  ap_ctrl_hs|  gv_reader_Pipeline_VITIS_LOOP_122_1|  return value|
|ap_idle                 |  out|    1|  ap_ctrl_hs|  gv_reader_Pipeline_VITIS_LOOP_122_1|  return value|
|ap_ready                |  out|    1|  ap_ctrl_hs|  gv_reader_Pipeline_VITIS_LOOP_122_1|  return value|
## gv_store_csynth.rpt
    |  Clock |  Target | Estimated| Uncertainty|
        |grp_gv_store_Pipeline_VITIS_LOOP_211_1_fu_93  |gv_store_Pipeline_VITIS_LOOP_211_1  |        ?|        ?|         ?|         ?|    0|    0|  l
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
|DSP                  |        -|     -|        -|       -|    -|
    |                   Instance                   |               Module               | BRAM_18K| DSP| FF | LUT | URAM|
    |grp_gv_store_Pipeline_VITIS_LOOP_211_1_fu_93  |gv_store_Pipeline_VITIS_LOOP_211_1  |        0|   0|  68|  126|    0|
    * DSP: 
    |     Variable Name    | Operation| DSP| FF| LUT| Bitwidth P0| Bitwidth P1|
    |grp_gv_store_Pipeline_VITIS_LOOP_211_1_fu_93_ap_start_reg  |   1|   0|    1|          0|
## gv_store_Pipeline_VITIS_LOOP_211_1_csynth.rpt
== Vitis HLS Report for 'gv_store_Pipeline_VITIS_LOOP_211_1'
    |  Clock |  Target | Estimated| Uncertainty|
        |- VITIS_LOOP_211_1  |        ?|        ?|         3|          1|          1|     ?|       yes|
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
|DSP                  |        -|     -|        -|       -|    -|
    * DSP: 
    |       Variable Name       | Operation| DSP| FF| LUT| Bitwidth P0| Bitwidth P1|
|ap_clk                  |   in|    1|  ap_ctrl_hs|  gv_store_Pipeline_VITIS_LOOP_211_1|  return value|
|ap_rst                  |   in|    1|  ap_ctrl_hs|  gv_store_Pipeline_VITIS_LOOP_211_1|  return value|
|ap_start                |   in|    1|  ap_ctrl_hs|  gv_store_Pipeline_VITIS_LOOP_211_1|  return value|
|ap_done                 |  out|    1|  ap_ctrl_hs|  gv_store_Pipeline_VITIS_LOOP_211_1|  return value|
|ap_idle                 |  out|    1|  ap_ctrl_hs|  gv_store_Pipeline_VITIS_LOOP_211_1|  return value|
|ap_ready                |  out|    1|  ap_ctrl_hs|  gv_store_Pipeline_VITIS_LOOP_211_1|  return value|
## gv_stream_Block_entry_proc_csynth.rpt
    |  Clock |  Target | Estimated| Uncertainty|
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
|DSP                  |        -|     -|        -|       -|    -|
    |          Instance         |        Module       | BRAM_18K| DSP|  FF | LUT| URAM|
    * DSP: 
    |  Variable Name  | Operation| DSP| FF| LUT| Bitwidth P0| Bitwidth P1|
## gv_stream_csynth.rpt
    |  Clock |  Target | Estimated| Uncertainty|
|         Name        | BRAM_18K|  DSP |    FF   |   LUT  | URAM|
|DSP                  |        -|     -|        -|       -|    -|
    |            Instance           |           Module           | BRAM_18K| DSP|   FF  |  LUT | URAM|
    * DSP: 
    |        Name       | BRAM_18K|  FF | LUT| URAM| Depth| Bits| Size:D*B|
    | Variable Name| Operation| DSP| FF| LUT| Bitwidth P0| Bitwidth P1|
[cs2401.02@rce report]$ 