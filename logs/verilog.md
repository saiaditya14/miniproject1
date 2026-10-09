[cs2401.02@rce verilog]$ cd ~/rce_upload/fpga_y_dummy16/build/hls/hls/syn/verilog; echo "== PARENT control"; grep -nE "ap_CS_fsm_state[0-9]+ *\)|ap_NS_fsm *=|ap_idle|ap_ready|ap_done|ap_continue|ap_start|empty_n|full_n|_read|_write|ap_condition|ap_sync" gemm_dataflow_parent_loop_proc3.v | grep -vE "^[0-9]+: *(input|output|wire|reg) " | cut -c1-165 | head -110; echo "== gemm.v: parent instance ports"; grep -n -A70 "^gemm_dataflow_parent_loop_proc3 " gemm.v | grep -vE "\.(At|Bbuf|Ct|sh)" | cut -c1-120 | head -70
== PARENT control
437:        ap_start,
438:        ap_done,
439:        ap_ready,
440:        ap_idle,
441:        ap_continue
1645:    .ap_start(dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_start),
1649:    .ap_done(dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_done),
1650:    .ap_ready(dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_ready),
1651:    .ap_idle(dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_idle),
1652:    .ap_continue(dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_continue)
1659:        if ((~(add_i2 <= ap_input_count_plus_step) & (ap_start == 1'b1) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_ready == 1'b1))) begin
1661:        end else if (((add_i2 <= ap_input_count_plus_step) & (ap_start == 1'b1) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_ready == 1'b1))) begin
1671:        if ((~(add_i2 <= ap_output_count_plus_step) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_continue == 1'b1) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_
1673:        end else if (((add_i2 <= ap_output_count_plus_step) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_continue == 1'b1) & (dataflow_in_loop_VITIS_LOOP_364_2_
1680:    if (((add_i2 <= ap_output_count_plus_step) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_done == 1'b1))) begin
1681:        ap_done = 1'b1;
1683:        ap_done = 1'b0;
1688:    if (((loop_dataflow_output_count == 32'd0) & (ap_start == 1'b0) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_idle == 1'b1))) begin
1689:        ap_idle = 1'b1;
1691:        ap_idle = 1'b0;
1696:    if (((add_i2 <= ap_input_count_plus_step) & (ap_start == 1'b1) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_ready == 1'b1))) begin
1697:        ap_ready = 1'b1;
1699:        ap_ready = 1'b0;
1704:    if ((~(add_i2 <= ap_output_count_plus_step) | (ap_continue == 1'b1))) begin
1705:        dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_continue = 1'b1;
1707:        dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_continue = 1'b0;
2227:assign dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_start = ap_start;
== gemm.v: parent instance ports
2142:gemm_dataflow_parent_loop_proc3 grp_dataflow_parent_loop_proc3_fu_412(
2143-    .add_i2(add_i2_reg_880),
2144-    .m_axi_gmem0_0_AWVALID(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_AWVALID),
2145-    .m_axi_gmem0_0_AWREADY(1'b0),
2146-    .m_axi_gmem0_0_AWADDR(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_AWADDR),
2147-    .m_axi_gmem0_0_AWID(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_AWID),
2148-    .m_axi_gmem0_0_AWLEN(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_AWLEN),
2149-    .m_axi_gmem0_0_AWSIZE(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_AWSIZE),
2150-    .m_axi_gmem0_0_AWBURST(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_AWBURST),
2151-    .m_axi_gmem0_0_AWLOCK(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_AWLOCK),
2152-    .m_axi_gmem0_0_AWCACHE(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_AWCACHE),
2153-    .m_axi_gmem0_0_AWPROT(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_AWPROT),
2154-    .m_axi_gmem0_0_AWQOS(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_AWQOS),
2155-    .m_axi_gmem0_0_AWREGION(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_AWREGION),
2156-    .m_axi_gmem0_0_AWUSER(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_AWUSER),
2157-    .m_axi_gmem0_0_WVALID(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_WVALID),
2158-    .m_axi_gmem0_0_WREADY(1'b0),
2159-    .m_axi_gmem0_0_WDATA(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_WDATA),
2160-    .m_axi_gmem0_0_WSTRB(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_WSTRB),
2161-    .m_axi_gmem0_0_WLAST(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_WLAST),
2162-    .m_axi_gmem0_0_WID(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_WID),
2163-    .m_axi_gmem0_0_WUSER(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_WUSER),
2164-    .m_axi_gmem0_0_ARVALID(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_ARVALID),
2165-    .m_axi_gmem0_0_ARREADY(gmem0_0_ARREADY),
2166-    .m_axi_gmem0_0_ARADDR(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_ARADDR),
2167-    .m_axi_gmem0_0_ARID(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_ARID),
2168-    .m_axi_gmem0_0_ARLEN(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_ARLEN),
2169-    .m_axi_gmem0_0_ARSIZE(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_ARSIZE),
2170-    .m_axi_gmem0_0_ARBURST(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_ARBURST),
2171-    .m_axi_gmem0_0_ARLOCK(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_ARLOCK),
2172-    .m_axi_gmem0_0_ARCACHE(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_ARCACHE),
2173-    .m_axi_gmem0_0_ARPROT(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_ARPROT),
2174-    .m_axi_gmem0_0_ARQOS(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_ARQOS),
2175-    .m_axi_gmem0_0_ARREGION(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_ARREGION),
2176-    .m_axi_gmem0_0_ARUSER(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_ARUSER),
2177-    .m_axi_gmem0_0_RVALID(gmem0_0_RVALID),
2178-    .m_axi_gmem0_0_RREADY(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_RREADY),
2179-    .m_axi_gmem0_0_RDATA(gmem0_0_RDATA),
2180-    .m_axi_gmem0_0_RLAST(1'b0),
2181-    .m_axi_gmem0_0_RID(1'd0),
2182-    .m_axi_gmem0_0_RFIFONUM(gmem0_0_RFIFONUM),
2183-    .m_axi_gmem0_0_RUSER(1'd0),
2184-    .m_axi_gmem0_0_RRESP(2'd0),
2185-    .m_axi_gmem0_0_BVALID(1'b0),
2186-    .m_axi_gmem0_0_BREADY(grp_dataflow_parent_loop_proc3_fu_412_m_axi_gmem0_0_BREADY),
2187-    .m_axi_gmem0_0_BRESP(2'd0),
2188-    .m_axi_gmem0_0_BID(1'd0),
2189-    .m_axi_gmem0_0_BUSER(1'd0),
2190-    .A(A_read_reg_858),
2191-    .M(M_read_reg_836),
2192-    .K(K_read_reg_815),
[cs2401.02@rce verilog]$ 

[cs2401.02@rce verilog]$ cd ~/rce_upload/fpga_y_dummy16/build/hls/hls/syn/verilog; echo "== gemm.v: loop bound add_i2"; grep -nE "add_i2" gemm.v | grep -vE "^[0-9]+: *(input|output|wire|reg) " | cut -c1-170 | head -20; echo "== parent: counters and step"; grep -nE "plus_step|loop_dataflow_(input|output)_count" gemm_dataflow_parent_loop_proc3.v | cut -c1-190 | head -30
== gemm.v: loop bound add_i2
2143:    .add_i2(add_i2_reg_880),
3309:        add_i2_reg_880 <= add_i2_fu_525_p2;
5031:assign add_i2_fu_525_p2 = (M + 32'd16);
== parent: counters and step
1207:reg   [31:0] loop_dataflow_input_count;
1208:reg   [31:0] loop_dataflow_output_count;
1209:wire   [31:0] ap_input_count_plus_step;
1210:wire   [31:0] ap_output_count_plus_step;
1215:#0 loop_dataflow_input_count = 32'd0;
1216:#0 loop_dataflow_output_count = 32'd0;
1269:    .i0(loop_dataflow_input_count),
1657:        loop_dataflow_input_count <= 32'd0;
1659:        if ((~(add_i2 <= ap_input_count_plus_step) & (ap_start == 1'b1) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_ready == 1'b1))) begin
1660:            loop_dataflow_input_count <= ap_input_count_plus_step;
1661:        end else if (((add_i2 <= ap_input_count_plus_step) & (ap_start == 1'b1) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_ready == 1'b1))) begin
1662:            loop_dataflow_input_count <= 32'd0;
1669:        loop_dataflow_output_count <= 32'd0;
1671:        if ((~(add_i2 <= ap_output_count_plus_step) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_continue == 1'b1) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_done == 1'b1))) begin
1672:            loop_dataflow_output_count <= ap_output_count_plus_step;
1673:        end else if (((add_i2 <= ap_output_count_plus_step) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_continue == 1'b1) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_done == 1'b1))) b
1674:            loop_dataflow_output_count <= 32'd0;
1680:    if (((add_i2 <= ap_output_count_plus_step) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_done == 1'b1))) begin
1688:    if (((loop_dataflow_output_count == 32'd0) & (ap_start == 1'b0) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_idle == 1'b1))) begin
1696:    if (((add_i2 <= ap_input_count_plus_step) & (ap_start == 1'b1) & (dataflow_in_loop_VITIS_LOOP_364_2_1_U0_ap_ready == 1'b1))) begin
1704:    if ((~(add_i2 <= ap_output_count_plus_step) | (ap_continue == 1'b1))) begin
2223:assign ap_input_count_plus_step = (loop_dataflow_input_count + 32'd16);
2225:assign ap_output_count_plus_step = (loop_dataflow_output_count + 32'd16);
[cs2401.02@rce verilog]$ ^C
[cs2401.02@rce verilog]$ 