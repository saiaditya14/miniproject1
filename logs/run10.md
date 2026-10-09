[cs2401.02@rce verilog]$ cd ~/rce_upload/fpga_y_dummy16/build/hls/hls/syn/verilog; echo "== STORE (i0 use + handshakes)"; grep -nE "\bi0\b|i0_read|i0_blk|ap_ready|ap_start|ap_done|ap_idle" gemm_tl_store9.v | cut -c1-170 | head -70; echo "== LOAD (i0 push + handshakes)"; grep -nE "i0_c|ap_done|ap_ready|ap_continue|ap_start|ap_idle" gemm_tl_load7.v | cut -c1-170 | head -60; echo "== WIRING in tile loop"; grep -nE "i0_c|tl_store9_U0_i0|tl_store9_U0_ap_ready|ap_channel_done_At|tl_load7_U0_ap_done|tl_compute8_U0_ap_done" gemm_dataflow_in_loop_VITIS_LOOP_364_2_1.v | cut -c1-170 | head -50
== STORE (i0 use + handshakes)
12:        ap_start,
13:        ap_done,
15:        ap_idle,
16:        ap_ready,
578:        i0_read,
670:input   ap_start;
671:output   ap_done;
673:output   ap_idle;
674:output   ap_ready;
1236:output   i0_read;
1243:reg ap_done;
1244:reg ap_idle;
1245:reg ap_ready;
1265:reg i0_read;
1267:reg    ap_done_reg;
1275:reg    i0_blk_n;
1830:wire    grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_start;
1831:wire    grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_done;
1832:wire    grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_idle;
1833:wire    grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_ready;
1866:wire    grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_start;
1867:wire    grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_done;
1868:wire    grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_idle;
1869:wire    grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_ready;
1902:reg    grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_start_reg;
1904:reg    grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_start_reg;
2024:#0 ap_done_reg = 1'b0;
2026:#0 grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_start_reg = 1'b0;
2027:#0 grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_start_reg = 1'b0;
2033:    .ap_start(grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_start),
2034:    .ap_done(grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_done),
2035:    .ap_idle(grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_idle),
2036:    .ap_ready(grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_ready),
2607:    .ap_start(grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_start),
2608:    .ap_done(grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_done),
2609:    .ap_idle(grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_idle),
2610:    .ap_ready(grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_ready),
3229:        ap_done_reg <= 1'b0;
3232:            ap_done_reg <= 1'b0;
3234:            ap_done_reg <= 1'b1;
3241:        grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_start_reg <= 1'b0;
3244:            grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_start_reg <= 1'b1;
3245:        end else if ((grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_ready == 1'b1)) begin
3246:            grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_start_reg <= 1'b0;
3253:        grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_start_reg <= 1'b0;
3256:            grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_start_reg <= 1'b1;
3257:        end else if ((grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_ready == 1'b1)) begin
3258:            grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_start_reg <= 1'b0;
3848:    if ((grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_done == 1'b0)) begin
4018:    if ((grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_done == 1'b0)) begin
4029:        ap_done = 1'b1;
4031:        ap_done = ap_done_reg;
4036:    if (((ap_start == 1'b0) & (1'b1 == ap_CS_fsm_state1))) begin
4037:        ap_idle = 1'b1;
4039:        ap_idle = 1'b0;
4045:        ap_ready = 1'b1;
4047:        ap_ready = 1'b0;
4068:    if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0)) & (1'b1 == ap_CS_fsm_state1))) begin
4069:        i0_blk_n = i0_empty_n;
4071:        i0_blk_n = 1'b1;
4077:        i0_read = 1'b1;
4079:        i0_read = 1'b0;
4313:            if (((1'b1 == ap_CS_fsm_state8) & (grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_done == 1'b1))) begin
4336:            if (((grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_done == 1'b1) & (1'b1 == ap_CS_fsm_state13))) begin
4591:    ap_block_state1 = ((ap_done_reg == 1'b1) | (ap_start == 1'b0) | (i0_empty_n == 1'b0));
4624:assign grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_start = grp_tl_store9_Pipeline_VITIS_LOOP_340_1_fu_4756_ap_start_reg;
4626:assign grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_start = grp_tl_store9_Pipeline_VITIS_LOOP_346_2_VITIS_LOOP_347_3_fu_4231_ap_start_reg;
== LOAD (i0 push + handshakes)
12:        ap_start,
13:        ap_done,
14:        ap_continue,
15:        ap_idle,
16:        ap_ready,
131:        i0_c_din,
132:        i0_c_full_n,
133:        i0_c_write,
134:        i0_c_num_data_valid,
135:        i0_c_fifo_cap
218:input   ap_start;
219:output   ap_done;
220:input   ap_continue;
221:output   ap_idle;
222:output   ap_ready;
337:output  [31:0] i0_c_din;
338:input   i0_c_full_n;
339:output   i0_c_write;
340:input  [2:0] i0_c_num_data_valid;
341:input  [2:0] i0_c_fifo_cap;
343:reg ap_done;
344:reg ap_idle;
345:reg ap_ready;
359:reg i0_c_write;
361:reg    ap_done_reg;
366:reg    i0_c_blk_n;
386:wire    grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_start;
387:wire    grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_done;
388:wire    grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_idle;
389:wire    grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_ready;
486:reg    grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_start_reg;
583:#0 ap_done_reg = 1'b0;
585:#0 grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_start_reg = 1'b0;
591:    .ap_start(grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_start),
592:    .ap_done(grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_done),
593:    .ap_idle(grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_idle),
594:    .ap_ready(grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_ready),
750:        ap_done_reg <= 1'b0;
752:        if ((ap_continue == 1'b1)) begin
753:            ap_done_reg <= 1'b0;
754:        end else if (((grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_done == 1'b1) & (1'b1 == ap_CS_fsm_state77))) begin
755:            ap_done_reg <= 1'b1;
762:        grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_start_reg <= 1'b0;
765:            grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_start_reg <= 1'b1;
766:        end else if ((grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_ready == 1'b1)) begin
767:            grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_start_reg <= 1'b0;
959:    if ((grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_done == 1'b0)) begin
973:    if (((grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_done == 1'b1) & (1'b1 == ap_CS_fsm_state77))) begin
974:        ap_done = 1'b1;
976:        ap_done = ap_done_reg;
981:    if (((ap_start == 1'b0) & (1'b1 == ap_CS_fsm_state1))) begin
982:        ap_idle = 1'b1;
984:        ap_idle = 1'b0;
989:    if (((grp_tl_load7_Pipeline_VITIS_LOOP_169_1_fu_144_ap_done == 1'b1) & (1'b1 == ap_CS_fsm_state77))) begin
990:        ap_ready = 1'b1;
992:        ap_ready = 1'b0;
1005:    if ((~((ap_done_reg == 1'b1) | (ap_start == 1'b0)) & (1'b1 == ap_CS_fsm_state1))) begin
1006:        i0_c_blk_n = i0_c_full_n;
1008:        i0_c_blk_n = 1'b1;
1014:        i0_c_write = 1'b1;
== WIRING in tile loop
913:wire    tl_load7_U0_ap_done;
1013:wire   [31:0] tl_load7_U0_i0_c_din;
1014:wire    tl_load7_U0_i0_c_write;
1015:wire    ap_channel_done_At;
1020:wire    tl_compute8_U0_ap_done;
1640:wire    tl_store9_U0_ap_ready;
1673:wire    tl_store9_U0_i0_read;
1706:wire    i0_c_full_n;
1707:wire   [31:0] i0_c_dout;
1708:wire    i0_c_empty_n;
1709:wire   [2:0] i0_c_num_data_valid;
1710:wire   [2:0] i0_c_fifo_cap;
4298:    .i_write(ap_channel_done_At),
4323:    .i_write(ap_channel_done_At),
4348:    .i_write(ap_channel_done_At),
4373:    .i_write(ap_channel_done_At),
4398:    .i_write(ap_channel_done_At),
4423:    .i_write(ap_channel_done_At),
4448:    .i_write(ap_channel_done_At),
4473:    .i_write(ap_channel_done_At),
4498:    .i_write(ap_channel_done_At),
4523:    .i_write(ap_channel_done_At),
4548:    .i_write(ap_channel_done_At),
4573:    .i_write(ap_channel_done_At),
4598:    .i_write(ap_channel_done_At),
4623:    .i_write(ap_channel_done_At),
4648:    .i_write(ap_channel_done_At),
4673:    .i_write(ap_channel_done_At),
4682:    .ap_done(tl_load7_U0_ap_done),
4800:    .i0_c_din(tl_load7_U0_i0_c_din),
4801:    .i0_c_full_n(i0_c_full_n),
4802:    .i0_c_write(tl_load7_U0_i0_c_write),
4803:    .i0_c_num_data_valid(i0_c_num_data_valid),
4804:    .i0_c_fifo_cap(i0_c_fifo_cap)
4811:    .ap_done(tl_compute8_U0_ap_done),
5481:    .ap_ready(tl_store9_U0_ap_ready),
6041:    .i0_dout(i0_c_dout),
6042:    .i0_empty_n(i0_c_empty_n),
6043:    .i0_read(tl_store9_U0_i0_read),
6044:    .i0_num_data_valid(i0_c_num_data_valid),
6045:    .i0_fifo_cap(i0_c_fifo_cap),
6051:gemm_fifo_w32_d3_S i0_c_U(
6056:    .if_din(tl_load7_U0_i0_c_din),
6057:    .if_full_n(i0_c_full_n),
6058:    .if_write(tl_load7_U0_i0_c_write),
6059:    .if_dout(i0_c_dout),
6060:    .if_empty_n(i0_c_empty_n),
6061:    .if_read(tl_store9_U0_i0_read),
6062:    .if_num_data_valid(i0_c_num_data_valid),
6063:    .if_fifo_cap(i0_c_fifo_cap)
[cs2401.02@rce verilog]$ 