[cs2401.02@node06 ~]$ sinteractive -c 8 -A cs2401
bash: sinteractive: command not found
[cs2401.02@node06 ~]$ cd fpga && make csim
source /home/cs2401.02/fpga/env.sh >/dev/null; vitis-run --mode hls --csim --config hls_config.cfg --work_dir build/hls

****** vitis-run v2025.1 (64-bit)
  **** SW Build 6137779 on 2025-05-21-18:10:03
  **** Start of session at: Tue Oct  6 18:43:16 2026
    ** Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
    ** Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

  **** HLS Build v2025.1 6135595
INFO: [HLS 200-2005] Using work_dir /home/cs2401.02/fpga/build/hls 
INFO: [HLS 200-2006] Changing directory to /home/cs2401.02/fpga/build
INFO: [HLS 200-2176] Writing Vitis IDE component file /home/cs2401.02/fpga/build/hls/vitis-comp.json
INFO: [HLS 200-10] Creating and opening component '/home/cs2401.02/fpga/build/hls'.
INFO: [HLS 200-1505] Using default flow_target 'vivado'
Resolution: For help on HLS 200-1505 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-1505.html
INFO: [HLS 200-2174] Applying component config ini file /home/cs2401.02/fpga/hls_config.cfg
INFO: [HLS 200-1465] Applying config ini 'syn.file=src/kernel.cpp' from /home/cs2401.02/fpga/hls_config.cfg(7)
INFO: [HLS 200-10] Adding design file '/home/cs2401.02/fpga/src/kernel.cpp' to the project
INFO: [HLS 200-1465] Applying config ini 'tb.file=src/kernel_tb.cpp' from /home/cs2401.02/fpga/hls_config.cfg(9)
INFO: [HLS 200-10] Adding test bench file '/home/cs2401.02/fpga/src/kernel_tb.cpp' to the project
INFO: [HLS 200-1465] Applying config ini 'syn.top=gemm' from /home/cs2401.02/fpga/hls_config.cfg(8)
INFO: [HLS 200-1465] Applying config ini 'flow_target=vitis' from /home/cs2401.02/fpga/hls_config.cfg(5)
INFO: [HLS 200-1505] Using flow_target 'vitis'
Resolution: For help on HLS 200-1505 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-1505.html
INFO: [HLS 200-435] Setting 'open_solution -flow_target vitis' configuration: config_interface -m_axi_latency=64
INFO: [HLS 200-435] Setting 'open_solution -flow_target vitis' configuration: config_interface -m_axi_alignment_byte_size=64
INFO: [HLS 200-435] Setting 'open_solution -flow_target vitis' configuration: config_interface -m_axi_max_widen_bitwidth=512
INFO: [HLS 200-435] Setting 'open_solution -flow_target vitis' configuration: config_rtl -register_reset_num=3
INFO: [HLS 200-1465] Applying config ini 'part=xcu50-fsvh2104-2-e' from /home/cs2401.02/fpga/hls_config.cfg(2)
INFO: [HLS 200-1611] Setting target device to 'xcu50-fsvh2104-2-e'
INFO: [HLS 200-1465] Applying config ini 'clock=3.33ns' from /home/cs2401.02/fpga/hls_config.cfg(10)
INFO: [SYN 201-201] Setting up clock 'default' with a period of 3.33ns.
INFO: [HLS 200-1465] Applying config ini 'csim.argv=full' from /home/cs2401.02/fpga/hls_config.cfg(12)
INFO: [HLS 200-1465] Applying config ini 'package.output.format=xo' from /home/cs2401.02/fpga/hls_config.cfg(6)
INFO: [HLS 200-2176] Writing Vitis IDE component file /home/cs2401.02/fpga/build/hls/vitis-comp.json
INFO: [SIM 211-2] *************** CSIM start ***************
INFO: [SIM 211-4] CSIM will launch CLANG as the compiler.
INFO: [HLS 200-2036] Building debug C Simulation binaries
make[1]: Entering directory '/home/cs2401.02/fpga/build/hls/hls/csim/build'
   Compiling ../../../../../src/kernel_tb.cpp in debug mode
   Compiling ../../../../../src/kernel.cpp in debug mode
   Generating csim.exe
make[1]: Leaving directory '/home/cs2401.02/fpga/build/hls/hls/csim/build'
=== kernel_tb (full: target shapes) ===
  M=288    K=288  N=1    PASS  max|err|=2.65e-06
  M=288    K=288  N=32   PASS  max|err|=3.72e-06
  M=768    K=288  N=1    PASS  max|err|=2.37e-06
  M=768    K=288  N=32   PASS  max|err|=3.52e-06
  M=288    K=768  N=1    PASS  max|err|=5.83e-06
  M=288    K=768  N=32   PASS  max|err|=8.52e-06
  M=32000  K=288  N=1    PASS  max|err|=3.68e-06
  M=32000  K=288  N=32   PASS  max|err|=4.76e-06
  M=768    K=768  N=768  PASS  max|err|=1.07e-05
=== ALL PASS ===
INFO: [SIM 211-1] CSim done with 0 errors.
INFO: [SIM 211-3] *************** CSIM finish ***************
INFO: [HLS 200-112] Total CPU user time: 19.45 seconds. Total CPU system time: 0.79 seconds. Total elapsed time: 22.04 seconds; peak allocated memory: 447.125 MB.
INFO: [vitis-run 60-791] Total elapsed time: 0h 0m 26s
[cs2401.02@node06 fpga]$ make csynth
source /home/cs2401.02/fpga/env.sh >/dev/null; v++ -c --mode hls --config hls_config.cfg --work_dir build/hls

****** v++ v2025.1 (64-bit)
  **** SW Build 6137779 on 2025-05-21-18:10:03
  **** Start of session at: Tue Oct  6 18:43:44 2026
    ** Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
    ** Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

  **** HLS Build v2025.1 6135595
INFO: [HLS 200-2005] Using work_dir /home/cs2401.02/fpga/build/hls 
INFO: [HLS 200-2006] Changing directory to /home/cs2401.02/fpga/build
INFO: [HLS 200-2176] Writing Vitis IDE component file /home/cs2401.02/fpga/build/hls/vitis-comp.json
INFO: [HLS 200-10] Creating and opening component '/home/cs2401.02/fpga/build/hls'.
INFO: [HLS 200-1505] Using default flow_target 'vivado'
Resolution: For help on HLS 200-1505 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-1505.html
INFO: [HLS 200-2174] Applying component config ini file /home/cs2401.02/fpga/hls_config.cfg
INFO: [HLS 200-1465] Applying config ini 'syn.file=src/kernel.cpp' from /home/cs2401.02/fpga/hls_config.cfg(7)
INFO: [HLS 200-10] Adding design file '/home/cs2401.02/fpga/src/kernel.cpp' to the project
INFO: [HLS 200-1465] Applying config ini 'tb.file=src/kernel_tb.cpp' from /home/cs2401.02/fpga/hls_config.cfg(9)
INFO: [HLS 200-10] Adding test bench file '/home/cs2401.02/fpga/src/kernel_tb.cpp' to the project
INFO: [HLS 200-1465] Applying config ini 'syn.top=gemm' from /home/cs2401.02/fpga/hls_config.cfg(8)
INFO: [HLS 200-1465] Applying config ini 'flow_target=vitis' from /home/cs2401.02/fpga/hls_config.cfg(5)
INFO: [HLS 200-1505] Using flow_target 'vitis'
Resolution: For help on HLS 200-1505 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-1505.html
INFO: [HLS 200-435] Setting 'open_solution -flow_target vitis' configuration: config_interface -m_axi_latency=64
INFO: [HLS 200-435] Setting 'open_solution -flow_target vitis' configuration: config_interface -m_axi_alignment_byte_size=64
INFO: [HLS 200-435] Setting 'open_solution -flow_target vitis' configuration: config_interface -m_axi_max_widen_bitwidth=512
INFO: [HLS 200-435] Setting 'open_solution -flow_target vitis' configuration: config_rtl -register_reset_num=3
INFO: [HLS 200-1465] Applying config ini 'part=xcu50-fsvh2104-2-e' from /home/cs2401.02/fpga/hls_config.cfg(2)
INFO: [HLS 200-1611] Setting target device to 'xcu50-fsvh2104-2-e'
INFO: [HLS 200-1465] Applying config ini 'clock=3.33ns' from /home/cs2401.02/fpga/hls_config.cfg(10)
INFO: [SYN 201-201] Setting up clock 'default' with a period of 3.33ns.
INFO: [HLS 200-1465] Applying config ini 'csim.argv=full' from /home/cs2401.02/fpga/hls_config.cfg(12)
INFO: [HLS 200-1465] Applying config ini 'package.output.format=xo' from /home/cs2401.02/fpga/hls_config.cfg(6)
INFO: [HLS 200-2176] Writing Vitis IDE component file /home/cs2401.02/fpga/build/hls/vitis-comp.json
INFO: [HLS 200-111] Finished File checks and directory preparation: CPU user time: 1.81 seconds. CPU system time: 0.42 seconds. Elapsed time: 2.66 seconds; current allocated memory: 447.133 MB.
INFO: [HLS 200-10] Analyzing design file '../src/kernel.cpp' ... 
INFO: [HLS 200-111] Finished Source Code Analysis and Preprocessing: CPU user time: 0.33 seconds. CPU system time: 0.23 seconds. Elapsed time: 0.59 seconds; current allocated memory: 449.633 MB.
INFO: [HLS 200-777] Using interface defaults for 'Vitis' flow target.
INFO: [HLS 200-1995] There were 859 instructions in the design after the 'Compile/Link' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 13,222 instructions in the design after the 'Unroll/Inline (step 1)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 8,677 instructions in the design after the 'Unroll/Inline (step 2)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 8,622 instructions in the design after the 'Unroll/Inline (step 3)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 8,620 instructions in the design after the 'Unroll/Inline (step 4)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 10,648 instructions in the design after the 'Array/Struct (step 1)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 5,735 instructions in the design after the 'Array/Struct (step 2)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 5,735 instructions in the design after the 'Array/Struct (step 3)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 9,252 instructions in the design after the 'Array/Struct (step 4)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 7,171 instructions in the design after the 'Array/Struct (step 5)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 7,164 instructions in the design after the 'Performance' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 6,131 instructions in the design after the 'Performance (step 2)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 6,122 instructions in the design after the 'Performance (step 3)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 6,033 instructions in the design after the 'Performance (step 4)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 6,090 instructions in the design after the 'HW Transforms (step 1)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 200-1995] There were 5,059 instructions in the design after the 'HW Transforms (step 2)' phase of compilation. See the Design Size Report for more details (/home/cs2401.02/fpga/build/hls/hls/syn/report/csynth_design_size.rpt:2)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_128_6' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:128:20)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_144_9' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:144:20)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_172_15' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:172:21)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_174_16' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:174:21)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_179_17' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:179:36)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_181_18' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:181:21)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_188_19' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:188:21)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_192_20' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:192:36)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_195_21' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:195:40)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_198_22' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:198:44)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_44_5' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:44:19)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_63_8' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:63:19)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_67_9' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:67:34)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_69_10' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:69:20)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_76_11' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:76:35)
INFO: [HLS 214-291] Loop 'VITIS_LOOP_29_2' is marked as complete unroll implied by the pipeline pragma (../src/kernel.cpp:29:19)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_128_6' (../src/kernel.cpp:128:20) in function 'gemm_tiled' completely with a factor of 32 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_144_9' (../src/kernel.cpp:144:20) in function 'gemm_tiled' completely with a factor of 16 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_152_10' (../src/kernel.cpp:152:32) in function 'gemm_tiled' completely with a factor of 4 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_161_13' (../src/kernel.cpp:161:36) in function 'gemm_tiled' completely with a factor of 16 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_154_11' (../src/kernel.cpp:154:21) in function 'gemm_tiled' completely with a factor of 32 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_156_12' (../src/kernel.cpp:156:21) in function 'gemm_tiled' completely with a factor of 8 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_172_15' (../src/kernel.cpp:172:21) in function 'gemm_tiled' completely with a factor of 4 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_174_16' (../src/kernel.cpp:174:21) in function 'gemm_tiled' completely with a factor of 16 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_179_17' (../src/kernel.cpp:179:36) in function 'gemm_tiled' completely with a factor of 4 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_181_18' (../src/kernel.cpp:181:21) in function 'gemm_tiled' completely with a factor of 16 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_188_19' (../src/kernel.cpp:188:21) in function 'gemm_tiled' completely with a factor of 32 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_192_20' (../src/kernel.cpp:192:36) in function 'gemm_tiled' completely with a factor of 4 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_195_21' (../src/kernel.cpp:195:40) in function 'gemm_tiled' completely with a factor of 32 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_198_22' (../src/kernel.cpp:198:44) in function 'gemm_tiled' completely with a factor of 7 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_207_23' (../src/kernel.cpp:207:32) in function 'gemm_tiled' completely with a factor of 4 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_209_24' (../src/kernel.cpp:209:21) in function 'gemm_tiled' completely with a factor of 32 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_212_25' (../src/kernel.cpp:212:40) in function 'gemm_tiled' completely with a factor of 8 (../src/kernel.cpp:92:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_44_5' (../src/kernel.cpp:44:19) in function 'gemv' completely with a factor of 16 (../src/kernel.cpp:16:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_52_6' (../src/kernel.cpp:52:26) in function 'gemv' completely with a factor of 8 (../src/kernel.cpp:16:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_63_8' (../src/kernel.cpp:63:19) in function 'gemv' completely with a factor of 16 (../src/kernel.cpp:16:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_67_9' (../src/kernel.cpp:67:34) in function 'gemv' completely with a factor of 4 (../src/kernel.cpp:16:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_69_10' (../src/kernel.cpp:69:20) in function 'gemv' completely with a factor of 1 (../src/kernel.cpp:16:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_69_10' (../src/kernel.cpp:69:20) in function 'gemv' completely with a factor of 2 (../src/kernel.cpp:16:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_69_10' (../src/kernel.cpp:69:20) in function 'gemv' completely with a factor of 4 (../src/kernel.cpp:16:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_69_10' (../src/kernel.cpp:69:20) in function 'gemv' completely with a factor of 8 (../src/kernel.cpp:16:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_76_11' (../src/kernel.cpp:76:35) in function 'gemv' completely with a factor of 7 (../src/kernel.cpp:16:0)
INFO: [HLS 214-186] Unrolling loop 'VITIS_LOOP_29_2' (../src/kernel.cpp:29:19) in function 'gemv' completely with a factor of 16 (../src/kernel.cpp:16:0)
INFO: [HLS 214-178] Inlining function 'gemm_generic(float const*, float const*, float*, int, int, int)' into 'gemm' (../src/kernel.cpp:239:0)
INFO: [HLS 214-248] Applying array_partition to 'At': Complete partitioning on dimension 1. (../src/kernel.cpp:19:8)
INFO: [HLS 214-248] Applying array_partition to 'sh': Complete partitioning on dimension 1. (../src/kernel.cpp:22:8)
INFO: [HLS 214-248] Applying array_partition to 'Bbuf': Complete partitioning on dimension 2. (../src/kernel.cpp:93:11)
INFO: [HLS 214-248] Applying array_partition to 'At': Complete partitioning on dimension 1. (../src/kernel.cpp:95:8)
INFO: [HLS 214-248] Applying array_partition to 'Ct': Complete partitioning on dimension 1. Complete partitioning on dimension 2. (../src/kernel.cpp:98:8)
INFO: [HLS 214-248] Applying array_partition to 'sh': Complete partitioning on dimension 1. Complete partitioning on dimension 2. Complete partitioning on dimension 3. (../src/kernel.cpp:100:8)
INFO: [HLS 214-248] Applying array_reshape to 'Bv': Cyclic reshaping with factor 16 on dimension 1. (../src/kernel.cpp:17:11)
INFO: [HLS 214-248] Applying array_reshape to 'At_0': Cyclic reshaping with factor 16 on dimension 1. (../src/kernel.cpp:19:8)
INFO: [HLS 214-248] Applying array_reshape to 'At_1': Cyclic reshaping with factor 16 on dimension 1. (../src/kernel.cpp:19:8)
INFO: [HLS 214-248] Applying array_reshape to 'At_2': Cyclic reshaping with factor 16 on dimension 1. (../src/kernel.cpp:19:8)
INFO: [HLS 214-248] Applying array_reshape to 'At_3': Cyclic reshaping with factor 16 on dimension 1. (../src/kernel.cpp:19:8)
INFO: [HLS 214-248] Applying array_reshape to 'At_4': Cyclic reshaping with factor 16 on dimension 1. (../src/kernel.cpp:19:8)
INFO: [HLS 214-248] Applying array_reshape to 'At_5': Cyclic reshaping with factor 16 on dimension 1. (../src/kernel.cpp:19:8)
INFO: [HLS 214-248] Applying array_reshape to 'At_6': Cyclic reshaping with factor 16 on dimension 1. (../src/kernel.cpp:19:8)
INFO: [HLS 214-248] Applying array_reshape to 'At_7': Cyclic reshaping with factor 16 on dimension 1. (../src/kernel.cpp:19:8)
INFO: [HLS 214-248] Applying array_reshape to 'At_0': Cyclic reshaping with factor 16 on dimension 1. (../src/kernel.cpp:95:8)
INFO: [HLS 214-248] Applying array_reshape to 'At_1': Cyclic reshaping with factor 16 on dimension 1. (../src/kernel.cpp:95:8)
INFO: [HLS 214-248] Applying array_reshape to 'At_2': Cyclic reshaping with factor 16 on dimension 1. (../src/kernel.cpp:95:8)
INFO: [HLS 214-248] Applying array_reshape to 'At_3': Cyclic reshaping with factor 16 on dimension 1. (../src/kernel.cpp:95:8)
INFO: [HLS 214-376] automatically set the pipeline for Loop< VITIS_LOOP_10_3> at ../src/kernel.cpp:10:30 
INFO: [HLS 214-115] Multiple burst reads of variable length and bit width 512 in loop 'VITIS_LOOP_27_1'(../src/kernel.cpp:27:22) has been inferred on bundle 'gmem1'. These burst requests might be further partitioned into multiple requests during RTL generation, based on max_read_burst_length or max_write_burst_length settings. (../src/kernel.cpp:27:22)
INFO: [HLS 214-115] Multiple burst reads of variable length and bit width 512 in loop 'VITIS_LOOP_41_4'(../src/kernel.cpp:41:30) has been inferred on bundle 'gmem0'. These burst requests might be further partitioned into multiple requests during RTL generation, based on max_read_burst_length or max_write_burst_length settings. (../src/kernel.cpp:41:30)
INFO: [HLS 214-115] Multiple burst writes of variable length and bit width 32 in loop 'VITIS_LOOP_85_12'(../src/kernel.cpp:85:27) has been inferred on bundle 'gmem2'. These burst requests might be further partitioned into multiple requests during RTL generation, based on max_read_burst_length or max_write_burst_length settings. (../src/kernel.cpp:85:27)
INFO: [HLS 214-115] Multiple burst reads of variable length and bit width 512 in loop 'VITIS_LOOP_141_8'(../src/kernel.cpp:141:35) has been inferred on bundle 'gmem0'. These burst requests might be further partitioned into multiple requests during RTL generation, based on max_read_burst_length or max_write_burst_length settings. (../src/kernel.cpp:141:35)
INFO: [HLS 214-115] Multiple burst writes of variable length and bit width 32 in loop 'VITIS_LOOP_222_26'(../src/kernel.cpp:222:36) has been inferred on bundle 'gmem2'. These burst requests might be further partitioned into multiple requests during RTL generation, based on max_read_burst_length or max_write_burst_length settings. (../src/kernel.cpp:222:36)
INFO: [HLS 214-115] Multiple burst writes of variable length and bit width 32 in loop 'VITIS_LOOP_229_28'(../src/kernel.cpp:229:40) has been inferred on bundle 'gmem2'. These burst requests might be further partitioned into multiple requests during RTL generation, based on max_read_burst_length or max_write_burst_length settings. (../src/kernel.cpp:229:40)
INFO: [HLS 214-115] Multiple burst writes of variable length and bit width 32 has been inferred on bundle 'gmem2'. These burst requests might be further partitioned into multiple requests during RTL generation, based on max_read_burst_length or max_write_burst_length settings. (../src/kernel.cpp:7:21)
INFO: [HLS 200-111] Finished Compiling Optimization and Transform: CPU user time: 24.59 seconds. CPU system time: 0.42 seconds. Elapsed time: 29.08 seconds; current allocated memory: 458.852 MB.
INFO: [HLS 200-111] Finished Checking Pragmas: CPU user time: 0 seconds. CPU system time: 0 seconds. Elapsed time: 0 seconds; current allocated memory: 458.852 MB.
INFO: [HLS 200-10] Starting code transformations ...
INFO: [HLS 200-111] Finished Standard Transforms: CPU user time: 0.19 seconds. CPU system time: 0 seconds. Elapsed time: 0.2 seconds; current allocated memory: 463.078 MB.
INFO: [HLS 200-10] Checking synthesizability ...
INFO: [HLS 200-111] Finished Checking Synthesizability: CPU user time: 0.13 seconds. CPU system time: 0 seconds. Elapsed time: 0.13 seconds; current allocated memory: 465.645 MB.
INFO: [XFORM 203-401] Performing if-conversion on hyperblock from (../src/kernel.cpp:168:9) to (../src/kernel.cpp:167:32) in function 'gemm_tiled'... converting 3 basic blocks.
INFO: [HLS 200-111] Finished Loop, function and other optimizations: CPU user time: 0.82 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.84 seconds; current allocated memory: 491.723 MB.
INFO: [HLS 200-2061] Successfully converted nested loops 'VITIS_LOOP_118_3'(../src/kernel.cpp:118:31) and 'VITIS_LOOP_119_4'(../src/kernel.cpp:119:35) in function 'gemm_tiled' into perfectly nested loops.
INFO: [HLS 200-2061] Successfully converted nested loops 'VITIS_LOOP_228_27'(../src/kernel.cpp:228:36) and 'VITIS_LOOP_229_28'(../src/kernel.cpp:229:40) in function 'gemm_tiled' into perfectly nested loops.
INFO: [HLS 200-2060] Failed to convert nested loops 'VITIS_LOOP_8_2'(../src/kernel.cpp:8:25) and 'VITIS_LOOP_10_3'(../src/kernel.cpp:10:30) in function 'gemm' into perfectly nested loops.
INFO: [XFORM 203-541] Flattening a loop nest 'VITIS_LOOP_118_3' (../src/kernel.cpp:118:31) in function 'gemm_tiled'.
INFO: [XFORM 203-541] Flattening a loop nest 'VITIS_LOOP_228_27' (../src/kernel.cpp:228:36) in function 'gemm_tiled'.
WARNING: [HLS 200-960] Cannot flatten loop 'VITIS_LOOP_8_2' (../src/kernel.cpp:8:25) in function 'gemm' the outer loop is not a perfect loop because there is nontrivial logic in the loop latch.
Resolution: For help on HLS 200-960 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-960.html
INFO: [HLS 200-472] Inferring partial write operation for 'At' (../src/kernel.cpp:46:20)
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.7' (../src/kernel.cpp:19).
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.6' (../src/kernel.cpp:19).
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.5' (../src/kernel.cpp:19).
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.4' (../src/kernel.cpp:19).
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.3' (../src/kernel.cpp:19).
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.2' (../src/kernel.cpp:19).
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.1' (../src/kernel.cpp:19).
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At' (../src/kernel.cpp:19).
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.10' (../src/kernel.cpp:95).
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.9' (../src/kernel.cpp:95).
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.8' (../src/kernel.cpp:95).
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At' (../src/kernel.cpp:95).
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.7'.
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.6'.
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.5'.
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.4'.
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.3'.
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.2'.
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.1'.
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At'.
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.10'.
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.9'.
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At.8'.
WARNING: [ANALYSIS 214-52] Found false inter dependency for variable 'At'.
INFO: [HLS 200-111] Finished Architecture Synthesis: CPU user time: 1.3 seconds. CPU system time: 0.04 seconds. Elapsed time: 1.34 seconds; current allocated memory: 657.688 MB.
INFO: [HLS 200-10] Starting hardware synthesis ...
INFO: [HLS 200-10] Synthesizing 'gemm' ...
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemm_tiled_Pipeline_VITIS_LOOP_118_3_VITIS_LOOP_119_4' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-61] Pipelining loop 'VITIS_LOOP_118_3_VITIS_LOOP_119_4'.
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 80, loop 'VITIS_LOOP_118_3_VITIS_LOOP_119_4'
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 8.81 seconds. CPU system time: 0.03 seconds. Elapsed time: 8.89 seconds; current allocated memory: 662.504 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0.03 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.04 seconds; current allocated memory: 662.504 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemm_tiled_Pipeline_VITIS_LOOP_112_2' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-61] Pipelining loop 'VITIS_LOOP_112_2'.
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 75, loop 'VITIS_LOOP_112_2'
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 0.21 seconds. CPU system time: 0 seconds. Elapsed time: 0.23 seconds; current allocated memory: 663.992 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0.03 seconds. CPU system time: 0 seconds. Elapsed time: 0.04 seconds; current allocated memory: 663.992 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemm_tiled_Pipeline_VITIS_LOOP_126_5' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-61] Pipelining loop 'VITIS_LOOP_126_5'.
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 2, loop 'VITIS_LOOP_126_5'
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 0.21 seconds. CPU system time: 0 seconds. Elapsed time: 0.22 seconds; current allocated memory: 665.715 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0.01 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.03 seconds; current allocated memory: 665.715 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemm_tiled_Pipeline_VITIS_LOOP_141_8' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-61] Pipelining loop 'VITIS_LOOP_141_8'.
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 3, loop 'VITIS_LOOP_141_8'
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 0.06 seconds. CPU system time: 0 seconds. Elapsed time: 0.06 seconds; current allocated memory: 665.715 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0.01 seconds. CPU system time: 0 seconds. Elapsed time: 0.02 seconds; current allocated memory: 665.715 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemm_tiled_Pipeline_VITIS_LOOP_167_14' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-61] Pipelining loop 'VITIS_LOOP_167_14'.
WARNING: [HLS 200-887] Cannot meet target clock period from 'store' operation 0 bit ('empty_161_write_ln0') of variable 'p_load1452' on local variable 'empty_161' to 'fadd' operation 32 bit ('cur', ../src/kernel.cpp:197) (combination delay: 2.729 ns) to honor II or Latency constraint in region 'VITIS_LOOP_167_14'.
Resolution: For help on HLS 200-887 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-887.html
WARNING: [HLS 200-887] Cannot meet target clock period from 'fadd' operation 32 bit ('cur', ../src/kernel.cpp:197) to 'store' operation 0 bit ('empty_269_write_ln0') of variable 'add260_2262479_load' on local variable 'empty_269' (combination delay: 3.116 ns) to honor II or Latency constraint in region 'VITIS_LOOP_167_14'.
Resolution: For help on HLS 200-887 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-887.html
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 13, loop 'VITIS_LOOP_167_14'
WARNING: [HLS 200-871] Estimated clock period (2.729 ns) exceeds the target (target clock period: 3.330 ns, clock uncertainty: 0.899 ns, effective delay budget: 2.431 ns).
Resolution: For help on HLS 200-871 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-871.html
WARNING: [HLS 200-1016] The critical path in module 'gemm_tiled_Pipeline_VITIS_LOOP_167_14' consists of the following:
        'fadd' operation 32 bit ('cur', ../src/kernel.cpp:197) [4851]  (2.342 ns)
        'store' operation 0 bit ('empty_1020_write_ln197', ../src/kernel.cpp:197) of variable 'cur', ../src/kernel.cpp:197 on local variable 'empty_1020' [4852]  (0.387 ns)

Resolution: For help on HLS 200-1016 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-1016.html
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 4.18 seconds. CPU system time: 0.03 seconds. Elapsed time: 4.23 seconds; current allocated memory: 722.277 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0.92 seconds. CPU system time: 0.01 seconds. Elapsed time: 1.08 seconds; current allocated memory: 722.277 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemm_tiled_Pipeline_VITIS_LOOP_228_27_VITIS_LOOP_229_28' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-61] Pipelining loop 'VITIS_LOOP_228_27_VITIS_LOOP_229_28'.
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 77, loop 'VITIS_LOOP_228_27_VITIS_LOOP_229_28'
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 0.42 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.66 seconds; current allocated memory: 722.277 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0.03 seconds. CPU system time: 0 seconds. Elapsed time: 0.04 seconds; current allocated memory: 722.277 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemm_tiled_Pipeline_VITIS_LOOP_222_26' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-61] Pipelining loop 'VITIS_LOOP_222_26'.
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 3, loop 'VITIS_LOOP_222_26'
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 0.09 seconds. CPU system time: 0 seconds. Elapsed time: 0.11 seconds; current allocated memory: 722.277 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0.01 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.03 seconds; current allocated memory: 722.277 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemm_tiled' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 1.48 seconds. CPU system time: 0.01 seconds. Elapsed time: 1.5 seconds; current allocated memory: 738.430 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 1.61 seconds. CPU system time: 0.01 seconds. Elapsed time: 1.74 seconds; current allocated memory: 741.594 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemv_Pipeline_VITIS_LOOP_27_1' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-61] Pipelining loop 'VITIS_LOOP_27_1'.
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 3, loop 'VITIS_LOOP_27_1'
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 0.32 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.51 seconds; current allocated memory: 743.637 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0 seconds. CPU system time: 0 seconds. Elapsed time: 0.01 seconds; current allocated memory: 743.637 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemv_Pipeline_VITIS_LOOP_41_4' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-61] Pipelining loop 'VITIS_LOOP_41_4'.
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 4, loop 'VITIS_LOOP_41_4'
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 0.08 seconds. CPU system time: 0 seconds. Elapsed time: 0.08 seconds; current allocated memory: 744.562 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0.01 seconds. CPU system time: 0 seconds. Elapsed time: 0.02 seconds; current allocated memory: 744.637 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemv_Pipeline_VITIS_LOOP_59_7' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-61] Pipelining loop 'VITIS_LOOP_59_7'.
WARNING: [HLS 200-887] Cannot meet target clock period from 'store' operation 0 bit ('empty_write_ln0') of variable 'p_load26' on local variable 'empty' to 'fadd' operation 32 bit ('cur', ../src/kernel.cpp:75) (combination delay: 2.697 ns) to honor II or Latency constraint in region 'VITIS_LOOP_59_7'.
Resolution: For help on HLS 200-887 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-887.html
WARNING: [HLS 200-887] Cannot meet target clock period from 'fadd' operation 32 bit ('cur', ../src/kernel.cpp:75) to 'store' operation 0 bit ('empty_57_write_ln0') of variable 'p_load' on local variable 'empty_57' (combination delay: 2.857 ns) to honor II or Latency constraint in region 'VITIS_LOOP_59_7'.
Resolution: For help on HLS 200-887 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-887.html
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 40, loop 'VITIS_LOOP_59_7'
WARNING: [HLS 200-871] Estimated clock period (2.470 ns) exceeds the target (target clock period: 3.330 ns, clock uncertainty: 0.899 ns, effective delay budget: 2.431 ns).
Resolution: For help on HLS 200-871 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-871.html
WARNING: [HLS 200-1016] The critical path in module 'gemv_Pipeline_VITIS_LOOP_59_7' consists of the following:
        'fadd' operation 32 bit ('cur', ../src/kernel.cpp:75) [429]  (2.083 ns)
        'store' operation 0 bit ('empty_58_write_ln75', ../src/kernel.cpp:75) of variable 'cur', ../src/kernel.cpp:75 on local variable 'empty_58' [435]  (0.387 ns)

Resolution: For help on HLS 200-1016 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-1016.html
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 0.19 seconds. CPU system time: 0 seconds. Elapsed time: 0.2 seconds; current allocated memory: 747.316 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0.04 seconds. CPU system time: 0 seconds. Elapsed time: 0.07 seconds; current allocated memory: 747.391 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemv_Pipeline_VITIS_LOOP_85_12' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-61] Pipelining loop 'VITIS_LOOP_85_12'.
INFO: [HLS 200-1470] Pipelining result : Target II = 1, Final II = 1, Depth = 2, loop 'VITIS_LOOP_85_12'
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 0.04 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.06 seconds; current allocated memory: 747.824 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0.01 seconds. CPU system time: 0 seconds. Elapsed time: 0.01 seconds; current allocated memory: 748.504 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemv' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 0.05 seconds. CPU system time: 0 seconds. Elapsed time: 0.06 seconds; current allocated memory: 748.766 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0.06 seconds. CPU system time: 0 seconds. Elapsed time: 0.07 seconds; current allocated memory: 749.262 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemm_Pipeline_VITIS_LOOP_10_3' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-61] Pipelining loop 'VITIS_LOOP_10_3'.
WARNING: [HLS 200-880] The II Violation in module 'gemm_Pipeline_VITIS_LOOP_10_3' (loop 'VITIS_LOOP_10_3'): Unable to enforce a carried dependence constraint (II = 1, distance = 1, offset = 1) between 'store' operation 0 bit ('acc_write_ln9', ../src/kernel.cpp:9->../src/kernel.cpp:249) of variable 'acc', ../src/kernel.cpp:11->../src/kernel.cpp:249 on local variable 'acc', ../src/kernel.cpp:9->../src/kernel.cpp:249 and 'fadd' operation 32 bit ('acc', ../src/kernel.cpp:11->../src/kernel.cpp:249).
Resolution: For help on HLS 200-880 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-880.html
WARNING: [HLS 200-880] The II Violation in module 'gemm_Pipeline_VITIS_LOOP_10_3' (loop 'VITIS_LOOP_10_3'): Unable to enforce a carried dependence constraint (II = 2, distance = 1, offset = 1) between 'store' operation 0 bit ('acc_write_ln9', ../src/kernel.cpp:9->../src/kernel.cpp:249) of variable 'acc', ../src/kernel.cpp:11->../src/kernel.cpp:249 on local variable 'acc', ../src/kernel.cpp:9->../src/kernel.cpp:249 and 'fadd' operation 32 bit ('acc', ../src/kernel.cpp:11->../src/kernel.cpp:249).
Resolution: For help on HLS 200-880 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-880.html
WARNING: [HLS 200-880] The II Violation in module 'gemm_Pipeline_VITIS_LOOP_10_3' (loop 'VITIS_LOOP_10_3'): Unable to enforce a carried dependence constraint (II = 3, distance = 1, offset = 1) between 'store' operation 0 bit ('acc_write_ln9', ../src/kernel.cpp:9->../src/kernel.cpp:249) of variable 'acc', ../src/kernel.cpp:11->../src/kernel.cpp:249 on local variable 'acc', ../src/kernel.cpp:9->../src/kernel.cpp:249 and 'fadd' operation 32 bit ('acc', ../src/kernel.cpp:11->../src/kernel.cpp:249).
Resolution: For help on HLS 200-880 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-880.html
WARNING: [HLS 200-880] The II Violation in module 'gemm_Pipeline_VITIS_LOOP_10_3' (loop 'VITIS_LOOP_10_3'): Unable to enforce a carried dependence constraint (II = 4, distance = 1, offset = 1) between 'store' operation 0 bit ('acc_write_ln9', ../src/kernel.cpp:9->../src/kernel.cpp:249) of variable 'acc', ../src/kernel.cpp:11->../src/kernel.cpp:249 on local variable 'acc', ../src/kernel.cpp:9->../src/kernel.cpp:249 and 'fadd' operation 32 bit ('acc', ../src/kernel.cpp:11->../src/kernel.cpp:249).
Resolution: For help on HLS 200-880 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-880.html
WARNING: [HLS 200-880] The II Violation in module 'gemm_Pipeline_VITIS_LOOP_10_3' (loop 'VITIS_LOOP_10_3'): Unable to enforce a carried dependence constraint (II = 7, distance = 1, offset = 1) between 'store' operation 0 bit ('acc_write_ln9', ../src/kernel.cpp:9->../src/kernel.cpp:249) of variable 'acc', ../src/kernel.cpp:11->../src/kernel.cpp:249 on local variable 'acc', ../src/kernel.cpp:9->../src/kernel.cpp:249 and 'fadd' operation 32 bit ('acc', ../src/kernel.cpp:11->../src/kernel.cpp:249).
Resolution: For help on HLS 200-880 see docs.xilinx.com/access/sources/dita/topic?Doc_Version=2025.1%20English&url=ug1448-hls-guidance&resourceid=200-880.html
INFO: [HLS 200-1470] Pipelining result : Target II = NA, Final II = 8, Depth = 87, loop 'VITIS_LOOP_10_3'
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 0.06 seconds. CPU system time: 0 seconds. Elapsed time: 0.08 seconds; current allocated memory: 750.430 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0.03 seconds. CPU system time: 0 seconds. Elapsed time: 0.03 seconds; current allocated memory: 750.777 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-42] -- Implementing module 'gemm' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [SCHED 204-11] Starting scheduling ...
INFO: [SCHED 204-11] Finished scheduling.
INFO: [HLS 200-111] Finished Scheduling: CPU user time: 0.04 seconds. CPU system time: 0 seconds. Elapsed time: 0.05 seconds; current allocated memory: 751.812 MB.
INFO: [BIND 205-100] Starting micro-architecture generation ...
INFO: [BIND 205-101] Performing variable lifetime analysis.
INFO: [BIND 205-101] Exploring resource sharing.
INFO: [BIND 205-101] Binding ...
INFO: [BIND 205-100] Finished micro-architecture generation.
INFO: [HLS 200-111] Finished Binding: CPU user time: 0.27 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.27 seconds; current allocated memory: 752.047 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemm_tiled_Pipeline_VITIS_LOOP_118_3_VITIS_LOOP_119_4' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-1030] Apply Unified Pipeline Control on module 'gemm_tiled_Pipeline_VITIS_LOOP_118_3_VITIS_LOOP_119_4' pipeline 'VITIS_LOOP_118_3_VITIS_LOOP_119_4' pipeline type 'loop pipeline'
INFO: [RTGEN 206-100] Generating core module 'mul_31ns_32ns_62_2_1': 1 instance(s).
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemm_tiled_Pipeline_VITIS_LOOP_118_3_VITIS_LOOP_119_4'.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 0.18 seconds. CPU system time: 0 seconds. Elapsed time: 0.2 seconds; current allocated memory: 756.156 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemm_tiled_Pipeline_VITIS_LOOP_112_2' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-1030] Apply Unified Pipeline Control on module 'gemm_tiled_Pipeline_VITIS_LOOP_112_2' pipeline 'VITIS_LOOP_112_2' pipeline type 'loop pipeline'
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemm_tiled_Pipeline_VITIS_LOOP_112_2'.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 0.15 seconds. CPU system time: 0 seconds. Elapsed time: 0.19 seconds; current allocated memory: 761.527 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemm_tiled_Pipeline_VITIS_LOOP_126_5' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-1030] Apply Unified Pipeline Control on module 'gemm_tiled_Pipeline_VITIS_LOOP_126_5' pipeline 'VITIS_LOOP_126_5' pipeline type 'loop pipeline'
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemm_tiled_Pipeline_VITIS_LOOP_126_5'.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 0.1 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.14 seconds; current allocated memory: 767.438 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemm_tiled_Pipeline_VITIS_LOOP_141_8' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-1030] Apply Unified Pipeline Control on module 'gemm_tiled_Pipeline_VITIS_LOOP_141_8' pipeline 'VITIS_LOOP_141_8' pipeline type 'loop pipeline'
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_141_8/m_axi_gmem0_0_ARVALID' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_141_8/m_axi_gmem0_0_ARADDR' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_141_8/m_axi_gmem0_0_ARID' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_141_8/m_axi_gmem0_0_ARLEN' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_141_8/m_axi_gmem0_0_ARSIZE' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_141_8/m_axi_gmem0_0_ARBURST' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_141_8/m_axi_gmem0_0_ARLOCK' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_141_8/m_axi_gmem0_0_ARCACHE' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_141_8/m_axi_gmem0_0_ARPROT' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_141_8/m_axi_gmem0_0_ARQOS' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_141_8/m_axi_gmem0_0_ARREGION' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_141_8/m_axi_gmem0_0_ARUSER' to 0.
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemm_tiled_Pipeline_VITIS_LOOP_141_8'.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 0.06 seconds. CPU system time: 0 seconds. Elapsed time: 0.09 seconds; current allocated memory: 770.523 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemm_tiled_Pipeline_VITIS_LOOP_167_14' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-1030] Apply Unified Pipeline Control on module 'gemm_tiled_Pipeline_VITIS_LOOP_167_14' pipeline 'VITIS_LOOP_167_14' pipeline type 'loop pipeline'
INFO: [RTGEN 206-104] Estimated max fanout for 'gemm_tiled_Pipeline_VITIS_LOOP_167_14' is 85874 from HDL expression: (1'b0 == ap_block_pp0_stage0_11001)
INFO: [RTGEN 206-100] Generating core module 'fadd_32ns_32ns_32_5_no_dsp_1': 1 instance(s).
INFO: [RTGEN 206-100] Generating core module 'fadd_32ns_32ns_32_7_full_dsp_1': 127 instance(s).
INFO: [RTGEN 206-100] Generating core module 'fmul_32ns_32ns_32_4_max_dsp_1': 128 instance(s).
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemm_tiled_Pipeline_VITIS_LOOP_167_14'.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 1.94 seconds. CPU system time: 0.03 seconds. Elapsed time: 2 seconds; current allocated memory: 823.102 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemm_tiled_Pipeline_VITIS_LOOP_228_27_VITIS_LOOP_229_28' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-1030] Apply Unified Pipeline Control on module 'gemm_tiled_Pipeline_VITIS_LOOP_228_27_VITIS_LOOP_229_28' pipeline 'VITIS_LOOP_228_27_VITIS_LOOP_229_28' pipeline type 'loop pipeline'
INFO: [RTGEN 206-100] Generating core module 'mul_3ns_32ns_34_2_1': 1 instance(s).
INFO: [RTGEN 206-100] Generating core module 'sparsemux_65_5_32_1_1': 4 instance(s).
INFO: [RTGEN 206-100] Generating core module 'sparsemux_9_2_32_1_1': 1 instance(s).
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemm_tiled_Pipeline_VITIS_LOOP_228_27_VITIS_LOOP_229_28'.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 1.67 seconds. CPU system time: 0.08 seconds. Elapsed time: 2.28 seconds; current allocated memory: 937.609 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemm_tiled_Pipeline_VITIS_LOOP_222_26' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-1030] Apply Unified Pipeline Control on module 'gemm_tiled_Pipeline_VITIS_LOOP_222_26' pipeline 'VITIS_LOOP_222_26' pipeline type 'loop pipeline'
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_222_26/m_axi_gmem2_0_AWVALID' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_222_26/m_axi_gmem2_0_AWADDR' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_222_26/m_axi_gmem2_0_AWID' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_222_26/m_axi_gmem2_0_AWLEN' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_222_26/m_axi_gmem2_0_AWSIZE' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_222_26/m_axi_gmem2_0_AWBURST' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_222_26/m_axi_gmem2_0_AWLOCK' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_222_26/m_axi_gmem2_0_AWCACHE' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_222_26/m_axi_gmem2_0_AWPROT' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_222_26/m_axi_gmem2_0_AWQOS' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_222_26/m_axi_gmem2_0_AWREGION' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_222_26/m_axi_gmem2_0_AWUSER' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemm_tiled_Pipeline_VITIS_LOOP_222_26/m_axi_gmem2_0_BREADY' to 0.
INFO: [RTGEN 206-100] Generating core module 'sparsemux_65_5_32_1_1': 4 instance(s).
INFO: [RTGEN 206-100] Generating core module 'sparsemux_9_2_32_1_1': 1 instance(s).
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemm_tiled_Pipeline_VITIS_LOOP_222_26'.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 0.16 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.22 seconds; current allocated memory: 942.762 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemm_tiled' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [RTGEN 206-104] Estimated max fanout for 'gemm_tiled' is 8636 from HDL expression: (1'b1 == ap_CS_fsm_state89)
INFO: [RTGEN 206-100] Generating core module 'fadd_32ns_32ns_32_7_full_dsp_1': 128 instance(s).
INFO: [RTGEN 206-100] Generating core module 'fmul_32ns_32ns_32_4_max_dsp_1': 1 instance(s).
INFO: [RTGEN 206-100] Generating core module 'mul_29ns_28s_56_2_1': 1 instance(s).
INFO: [RTGEN 206-100] Generating core module 'mul_29ns_32ns_60_2_1': 1 instance(s).
INFO: [RTGEN 206-100] Generating core module 'mul_31ns_31ns_62_2_1': 1 instance(s).
INFO: [RTGEN 206-100] Generating core module 'mul_32s_28s_32_2_1': 1 instance(s).
INFO: [RTGEN 206-100] Generating core module 'mul_32s_32s_32_2_1': 1 instance(s).
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemm_tiled'.
INFO: [RTMG 210-278] Implementing memory 'gemm_gemm_tiled_Bbuf_RAM_AUTO_1R1W' using auto RAMs.
INFO: [RTMG 210-278] Implementing memory 'gemm_gemm_tiled_At_RAM_AUTO_1R1W' using auto RAMs.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 0.71 seconds. CPU system time: 0.04 seconds. Elapsed time: 1.08 seconds; current allocated memory: 978.699 MB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemv_Pipeline_VITIS_LOOP_27_1' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-1030] Apply Unified Pipeline Control on module 'gemv_Pipeline_VITIS_LOOP_27_1' pipeline 'VITIS_LOOP_27_1' pipeline type 'loop pipeline'
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_27_1/m_axi_gmem1_0_ARVALID' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_27_1/m_axi_gmem1_0_ARADDR' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_27_1/m_axi_gmem1_0_ARID' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_27_1/m_axi_gmem1_0_ARLEN' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_27_1/m_axi_gmem1_0_ARSIZE' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_27_1/m_axi_gmem1_0_ARBURST' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_27_1/m_axi_gmem1_0_ARLOCK' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_27_1/m_axi_gmem1_0_ARCACHE' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_27_1/m_axi_gmem1_0_ARPROT' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_27_1/m_axi_gmem1_0_ARQOS' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_27_1/m_axi_gmem1_0_ARREGION' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_27_1/m_axi_gmem1_0_ARUSER' to 0.
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemv_Pipeline_VITIS_LOOP_27_1'.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 0.88 seconds. CPU system time: 0.05 seconds. Elapsed time: 1.28 seconds; current allocated memory: 1.036 GB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemv_Pipeline_VITIS_LOOP_41_4' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-1030] Apply Unified Pipeline Control on module 'gemv_Pipeline_VITIS_LOOP_41_4' pipeline 'VITIS_LOOP_41_4' pipeline type 'loop pipeline'
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_41_4/m_axi_gmem0_0_ARVALID' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_41_4/m_axi_gmem0_0_ARADDR' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_41_4/m_axi_gmem0_0_ARID' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_41_4/m_axi_gmem0_0_ARLEN' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_41_4/m_axi_gmem0_0_ARSIZE' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_41_4/m_axi_gmem0_0_ARBURST' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_41_4/m_axi_gmem0_0_ARLOCK' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_41_4/m_axi_gmem0_0_ARCACHE' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_41_4/m_axi_gmem0_0_ARPROT' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_41_4/m_axi_gmem0_0_ARQOS' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_41_4/m_axi_gmem0_0_ARREGION' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_41_4/m_axi_gmem0_0_ARUSER' to 0.
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemv_Pipeline_VITIS_LOOP_41_4'.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 0.07 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.08 seconds; current allocated memory: 1.036 GB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemv_Pipeline_VITIS_LOOP_59_7' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-1030] Apply Unified Pipeline Control on module 'gemv_Pipeline_VITIS_LOOP_59_7' pipeline 'VITIS_LOOP_59_7' pipeline type 'loop pipeline'
INFO: [RTGEN 206-100] Generating core module 'fadd_32ns_32ns_32_5_no_dsp_1': 1 instance(s).
INFO: [RTGEN 206-100] Generating core module 'fadd_32ns_32ns_32_7_full_dsp_1': 15 instance(s).
INFO: [RTGEN 206-100] Generating core module 'fmul_32ns_32ns_32_4_max_dsp_1': 16 instance(s).
INFO: [RTGEN 206-100] Generating core module 'sparsemux_17_3_32_1_1': 16 instance(s).
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemv_Pipeline_VITIS_LOOP_59_7'.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 0.19 seconds. CPU system time: 0 seconds. Elapsed time: 0.23 seconds; current allocated memory: 1.036 GB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemv_Pipeline_VITIS_LOOP_85_12' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-1030] Apply Unified Pipeline Control on module 'gemv_Pipeline_VITIS_LOOP_85_12' pipeline 'VITIS_LOOP_85_12' pipeline type 'loop pipeline'
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_85_12/m_axi_gmem2_0_AWVALID' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_85_12/m_axi_gmem2_0_AWADDR' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_85_12/m_axi_gmem2_0_AWID' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_85_12/m_axi_gmem2_0_AWLEN' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_85_12/m_axi_gmem2_0_AWSIZE' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_85_12/m_axi_gmem2_0_AWBURST' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_85_12/m_axi_gmem2_0_AWLOCK' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_85_12/m_axi_gmem2_0_AWCACHE' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_85_12/m_axi_gmem2_0_AWPROT' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_85_12/m_axi_gmem2_0_AWQOS' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_85_12/m_axi_gmem2_0_AWREGION' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_85_12/m_axi_gmem2_0_AWUSER' to 0.
WARNING: [RTGEN 206-101] Setting dangling out port 'gemv_Pipeline_VITIS_LOOP_85_12/m_axi_gmem2_0_BREADY' to 0.
INFO: [RTGEN 206-100] Generating core module 'sparsemux_17_3_32_1_1': 1 instance(s).
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemv_Pipeline_VITIS_LOOP_85_12'.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 0.13 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.2 seconds; current allocated memory: 1.036 GB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemv' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [RTGEN 206-100] Generating core module 'mul_28ns_28s_55_2_1': 1 instance(s).
INFO: [RTGEN 206-100] Generating core module 'mul_32s_28s_32_2_1': 1 instance(s).
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemv'.
INFO: [RTMG 210-278] Implementing memory 'gemm_gemv_Bv_RAM_AUTO_1R1W' using auto RAMs.
INFO: [RTMG 210-278] Implementing memory 'gemm_gemv_At_RAM_AUTO_1R1W' using auto RAMs.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 0.11 seconds. CPU system time: 0 seconds. Elapsed time: 0.12 seconds; current allocated memory: 1.036 GB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemm_Pipeline_VITIS_LOOP_10_3' 
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-1030] Apply Unified Pipeline Control on module 'gemm_Pipeline_VITIS_LOOP_10_3' pipeline 'VITIS_LOOP_10_3' pipeline type 'loop pipeline'
INFO: [RTGEN 206-100] Generating core module 'fadd_32ns_32ns_32_7_full_dsp_1': 1 instance(s).
INFO: [RTGEN 206-100] Generating core module 'fmul_32ns_32ns_32_4_max_dsp_1': 1 instance(s).
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemm_Pipeline_VITIS_LOOP_10_3'.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 0.16 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.2 seconds; current allocated memory: 1.036 GB.
INFO: [HLS 200-10] ----------------------------------------------------------------
INFO: [HLS 200-10] -- Generating RTL for module 'gemm' 
INFO: [HLS 200-10] ----------------------------------------------------------------
WARNING: [RTGEN 206-101] Design contains AXI ports. Reset is fixed to synchronous and active low.
INFO: [RTGEN 206-500] Setting interface mode on port 'gemm/gmem0' to 'm_axi'.
INFO: [RTGEN 206-500] Setting interface mode on port 'gemm/gmem1' to 'm_axi'.
INFO: [RTGEN 206-500] Setting interface mode on port 'gemm/gmem2' to 'm_axi'.
INFO: [RTGEN 206-500] Setting interface mode on port 'gemm/A' to 's_axilite & ap_none'.
INFO: [RTGEN 206-500] Setting interface mode on port 'gemm/B' to 's_axilite & ap_none'.
INFO: [RTGEN 206-500] Setting interface mode on port 'gemm/C' to 's_axilite & ap_none'.
INFO: [RTGEN 206-500] Setting interface mode on port 'gemm/M' to 's_axilite & ap_none'.
INFO: [RTGEN 206-500] Setting interface mode on port 'gemm/K' to 's_axilite & ap_none'.
INFO: [RTGEN 206-500] Setting interface mode on port 'gemm/N' to 's_axilite & ap_none'.
INFO: [RTGEN 206-500] Setting interface mode on function 'gemm' to 's_axilite & ap_ctrl_chain'.
INFO: [RTGEN 206-100] Bundling port 'A', 'B', 'C', 'M', 'K', 'N' and 'return' to AXI-Lite port control.
INFO: [RTGEN 206-100] Generating core module 'fadd_32ns_32ns_32_7_full_dsp_1': 1 instance(s).
INFO: [RTGEN 206-100] Generating core module 'fmul_32ns_32ns_32_4_max_dsp_1': 1 instance(s).
INFO: [RTGEN 206-100] Generating core module 'mul_32ns_32ns_63_2_1': 1 instance(s).
INFO: [RTGEN 206-100] Finished creating RTL model for 'gemm'.
INFO: [HLS 200-111] Finished Creating RTL model: CPU user time: 0.11 seconds. CPU system time: 0.01 seconds. Elapsed time: 0.17 seconds; current allocated memory: 1.038 GB.
INFO: [HLS 200-111] Finished Generating all RTL models: CPU user time: 0.73 seconds. CPU system time: 0.08 seconds. Elapsed time: 1.22 seconds; current allocated memory: 1.042 GB.
INFO: [HLS 200-1603] Design has inferred MAXI bursts and missed bursts, see Vitis HLS GUI synthesis summary report for detailed information.
INFO: [HLS 200-111] Finished Updating report files: CPU user time: 1.18 seconds. CPU system time: 0.05 seconds. Elapsed time: 1.31 seconds; current allocated memory: 1.073 GB.
INFO: [VHDL 208-304] Generating VHDL RTL for gemm.
INFO: [VLOG 209-307] Generating Verilog RTL for gemm.
INFO: [HLS 200-790] **** Loop Constraint Status: All loop constraints were NOT satisfied.
INFO: [HLS 200-789] **** Estimated Fmax: 366.43 MHz
INFO: [IMPL 213-8] Exporting RTL as a Vivado IP.

****** Vivado v2025.1 (64-bit)
  **** SW Build 6140274 on Wed May 21 22:58:25 MDT 2025
  **** IP Build 6138677 on Thu May 22 03:10:11 MDT 2025
  **** SharedData Build 6139179 on Tue May 20 17:58:58 MDT 2025
  **** Start of session at: Tue Oct  6 18:44:59 2026
    ** Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
    ** Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.

source run_ippack.tcl -notrace
INFO: calling package_hls_ip ip_types=vitis sysgen json_file=/home/cs2401.02/fpga/build/hls/hls/hls_data.json outdir=/home/cs2401.02/fpga/build/hls/hls/impl/ip srcdir=/home/cs2401.02/fpga/build/hls/hls ippack_options_dict= ippack_options_dict=
INFO: Copied 1 ipmisc file(s) to /home/cs2401.02/fpga/build/hls/hls/impl/ip/misc
INFO: Copied 39 verilog file(s) to /home/cs2401.02/fpga/build/hls/hls/impl/ip/hdl/verilog
INFO: Copied 39 vhdl file(s) to /home/cs2401.02/fpga/build/hls/hls/impl/ip/hdl/vhdl
INFO: Copied 10 swdriver file(s) to /home/cs2401.02/fpga/build/hls/hls/impl/ip/drivers
Generating 3 subcores in /home/cs2401.02/fpga/build/hls/hls/impl/ip/subcore_prj:
impl/misc/gemm_fadd_32ns_32ns_32_5_no_dsp_1_ip.tcl
impl/misc/gemm_fadd_32ns_32ns_32_7_full_dsp_1_ip.tcl
impl/misc/gemm_fmul_32ns_32ns_32_4_max_dsp_1_ip.tcl
WARNING: [Common 17-1496] Vivado tclapp support version doesn't have install/uninstall hooks. Please update support using ::tclapp::update_support command.
WARNING: [Common 17-1496] Vivado tclapp support version doesn't have install/uninstall hooks. Please update support using ::tclapp::update_support command.
WARNING: [Common 17-1496] Vivado tclapp support version doesn't have install/uninstall hooks. Please update support using ::tclapp::update_support command.
WARNING: [Common 17-1496] Vivado tclapp support version doesn't have install/uninstall hooks. Please update support using ::tclapp::update_support command.
WARNING: [Common 17-1496] Vivado tclapp support version doesn't have install/uninstall hooks. Please update support using ::tclapp::update_support command.
WARNING: [Common 17-1496] Vivado tclapp support version doesn't have install/uninstall hooks. Please update support using ::tclapp::update_support command.
INFO: Using COE_DIR=/home/cs2401.02/fpga/build/hls/hls/impl/ip/hdl/verilog
INFO: Generating gemm_fadd_32ns_32ns_32_5_no_dsp_1_ip via file impl/misc/gemm_fadd_32ns_32ns_32_5_no_dsp_1_ip.tcl
INFO: [IP_Flow 19-234] Refreshing IP repositories
INFO: [IP_Flow 19-1704] No user IP repositories specified
INFO: [IP_Flow 19-2313] Loaded Vivado IP repository '/home/Xilinx/2025.1/Vivado/data/ip'.
INFO: [IP_Flow 19-1686] Generating 'Synthesis' target for IP 'gemm_fadd_32ns_32ns_32_5_no_dsp_1_ip'...
INFO: [IP_Flow 19-1686] Generating 'Simulation' target for IP 'gemm_fadd_32ns_32ns_32_5_no_dsp_1_ip'...
INFO: Done generating gemm_fadd_32ns_32ns_32_5_no_dsp_1_ip via file impl/misc/gemm_fadd_32ns_32ns_32_5_no_dsp_1_ip.tcl
INFO: Generating gemm_fadd_32ns_32ns_32_7_full_dsp_1_ip via file impl/misc/gemm_fadd_32ns_32ns_32_7_full_dsp_1_ip.tcl
INFO: [IP_Flow 19-1686] Generating 'Synthesis' target for IP 'gemm_fadd_32ns_32ns_32_7_full_dsp_1_ip'...
INFO: [IP_Flow 19-1686] Generating 'Simulation' target for IP 'gemm_fadd_32ns_32ns_32_7_full_dsp_1_ip'...
INFO: Done generating gemm_fadd_32ns_32ns_32_7_full_dsp_1_ip via file impl/misc/gemm_fadd_32ns_32ns_32_7_full_dsp_1_ip.tcl
INFO: Generating gemm_fmul_32ns_32ns_32_4_max_dsp_1_ip via file impl/misc/gemm_fmul_32ns_32ns_32_4_max_dsp_1_ip.tcl
INFO: [IP_Flow 19-1686] Generating 'Synthesis' target for IP 'gemm_fmul_32ns_32ns_32_4_max_dsp_1_ip'...
INFO: [IP_Flow 19-1686] Generating 'Simulation' target for IP 'gemm_fmul_32ns_32ns_32_4_max_dsp_1_ip'...
INFO: Done generating gemm_fmul_32ns_32ns_32_4_max_dsp_1_ip via file impl/misc/gemm_fmul_32ns_32ns_32_4_max_dsp_1_ip.tcl
INFO: Found 1 unique subcore IP: xilinx.com:ip:floating_point:7.1
INFO: Copied 3 subcore files: /home/cs2401.02/fpga/build/hls/hls/impl/ip/hdl/ip/gemm_fmul_32ns_32ns_32_4_max_dsp_1_ip/gemm_fmul_32ns_32ns_32_4_max_dsp_1_ip.xci /home/cs2401.02/fpga/build/hls/hls/impl/ip/hdl/ip/gemm_fadd_32ns_32ns_32_7_full_dsp_1_ip/gemm_fadd_32ns_32ns_32_7_full_dsp_1_ip.xci /home/cs2401.02/fpga/build/hls/hls/impl/ip/hdl/ip/gemm_fadd_32ns_32ns_32_5_no_dsp_1_ip/gemm_fadd_32ns_32ns_32_5_no_dsp_1_ip.xci
INFO: Import ports from HDL: /home/cs2401.02/fpga/build/hls/hls/impl/ip/hdl/vhdl/gemm.vhd (gemm)
INFO: Add axi4lite interface s_axi_control
INFO: Add clock interface ap_clk
INFO: Add reset interface ap_rst_n
INFO: Add interrupt interface interrupt
INFO: [IP_Flow 19-234] Refreshing IP repositories
INFO: [IP_Flow 19-1704] No user IP repositories specified
INFO: [IP_Flow 19-2313] Loaded Vivado IP repository '/home/Xilinx/2025.1/Vivado/data/ip'.
INFO: Add axi4full interface m_axi_gmem0
INFO: Add axi4full interface m_axi_gmem1
INFO: Add axi4full interface m_axi_gmem2
INFO: Calling post_process_vitis to specialize IP
INFO: Calling post_process_sysgen to specialize IP
Generating sysgen info xml from json file
INFO: Created IP /home/cs2401.02/fpga/build/hls/hls/impl/ip/component.xml
Generating XO file: gemm.xo in directory /home/cs2401.02/fpga/build/hls/hls/..
Running: package_xo -xo_path /home/cs2401.02/fpga/build/hls/hls/../gemm.xo -kernel_xml /home/cs2401.02/fpga/build/hls/hls/kernel.xml -kernel_name gemm -ip_directory /home/cs2401.02/fpga/build/hls/hls/impl/ip -kernel_files /home/cs2401.02/fpga/build/hls/hls/../../../src/kernel.cpp -hls_directory /home/cs2401.02/fpga/build/hls/hls/impl/misc/hls_files -kernel_json /home/cs2401.02/fpga/build/hls/hls/hls_data.json
INFO: Created IP archive /home/cs2401.02/fpga/build/hls/hls/impl/ip/xilinx_com_hls_gemm_1_0.zip
INFO: [Common 17-206] Exiting Vivado at Tue Oct  6 18:45:10 2026...
INFO: [HLS 200-802] Generated output file hls/gemm.xo
INFO: [HLS 200-112] Total CPU user time: 79.85 seconds. Total CPU system time: 3.17 seconds. Total elapsed time: 101.55 seconds; peak allocated memory: 1.077 GB.
INFO: [v++ 60-791] Total elapsed time: 0h 1m 46s
csynth report: build/hls/hls/syn/report/gemm_csynth.rpt
[cs2401.02@node06 fpga]$ 