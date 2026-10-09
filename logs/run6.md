[cs2401.02@rce ~]$ for d in fpga_v5a fpga_v5b; do echo "##### $d"; cd ~/$d; grep -hiE "latency|cycles|PASS|FAIL|Total" build/logs/cosim-*.log | tail -40; cd ~; done
##### fpga_v5a
          rd_latency         integral                          32    'h0              
        write_latency_mode   latency_mode_enum                 32    TRANSACTION_FIRST
        read_latency_mode    latency_mode_enum                 32    TRANSACTION_FIRST
        clatency             axi_latency_gmem1                 -     @439             
          wr_latency         integral                          32    'h0              
          rd_latency         integral                          32    'h0              
        write_latency_mode   latency_mode_enum                 32    TRANSACTION_FIRST
        read_latency_mode    latency_mode_enum                 32    TRANSACTION_FIRST
        clatency             axi_latency_gmem2                 -     @471             
          wr_latency         integral                          32    'h0              
          rd_latency         integral                          32    'h0              
        write_latency_mode   latency_mode_enum                 32    TRANSACTION_FIRST
        read_latency_mode    latency_mode_enum                 32    TRANSACTION_FIRST
        clatency             axi_latency_gmem0                 -     @404             
          wr_latency         integral                          32    'h0              
          rd_latency         integral                          32    'h0              
        write_latency_mode   latency_mode_enum                 32    TRANSACTION_FIRST
        read_latency_mode    latency_mode_enum                 32    TRANSACTION_FIRST
        clatency             axi_latency_gmem1                 -     @439             
          wr_latency         integral                          32    'h0              
          rd_latency         integral                          32    'h0              
        write_latency_mode   latency_mode_enum                 32    TRANSACTION_FIRST
        read_latency_mode    latency_mode_enum                 32    TRANSACTION_FIRST
        clatency             axi_latency_gmem2                 -     @471             
          wr_latency         integral                          32    'h0              
          rd_latency         integral                          32    'h0              
        write_latency_mode   latency_mode_enum                 32    CHANNEL_FIRST    
        read_latency_mode    latency_mode_enum                 32    CHANNEL_FIRST    
        clatency             axi_latency                       -     @502             
          wr_latency         integral                          32    'h0              
          rd_latency         integral                          32    'h0              
// Inter-Transaction Progress: Completed Transaction / Total Transaction
// Intra-Transaction Progress: Measured Latency / Latency Estimation * 100%
  M=16     K=288  N=1    PASS  max|err|=1.50e-06
  M=8      K=768  N=1    PASS  max|err|=2.04e-06
  M=4      K=288  N=40   PASS  max|err|=1.98e-06
=== ALL PASS ===
INFO: [COSIM 212-1000] *** C/RTL co-simulation finished: PASS ***
INFO: [HLS 200-112] Total CPU user time: 679.31 seconds. Total CPU system time: 7.92 seconds. Total elapsed time: 451.5 seconds; peak allocated memory: 521.828 MB.
INFO: [vitis-run 60-791] Total elapsed time: 0h 7m 36s
##### fpga_v5b
          rd_latency         integral                          32    'h0              
        write_latency_mode   latency_mode_enum                 32    TRANSACTION_FIRST
        read_latency_mode    latency_mode_enum                 32    TRANSACTION_FIRST
        clatency             axi_latency_gmem1                 -     @439             
          wr_latency         integral                          32    'h0              
          rd_latency         integral                          32    'h0              
        write_latency_mode   latency_mode_enum                 32    TRANSACTION_FIRST
        read_latency_mode    latency_mode_enum                 32    TRANSACTION_FIRST
        clatency             axi_latency_gmem2                 -     @471             
          wr_latency         integral                          32    'h0              
          rd_latency         integral                          32    'h0              
        write_latency_mode   latency_mode_enum                 32    TRANSACTION_FIRST
        read_latency_mode    latency_mode_enum                 32    TRANSACTION_FIRST
        clatency             axi_latency_gmem0                 -     @404             
          wr_latency         integral                          32    'h0              
          rd_latency         integral                          32    'h0              
        write_latency_mode   latency_mode_enum                 32    TRANSACTION_FIRST
        read_latency_mode    latency_mode_enum                 32    TRANSACTION_FIRST
        clatency             axi_latency_gmem1                 -     @439             
          wr_latency         integral                          32    'h0              
          rd_latency         integral                          32    'h0              
        write_latency_mode   latency_mode_enum                 32    TRANSACTION_FIRST
        read_latency_mode    latency_mode_enum                 32    TRANSACTION_FIRST
        clatency             axi_latency_gmem2                 -     @471             
          wr_latency         integral                          32    'h0              
          rd_latency         integral                          32    'h0              
        write_latency_mode   latency_mode_enum                 32    CHANNEL_FIRST    
        read_latency_mode    latency_mode_enum                 32    CHANNEL_FIRST    
        clatency             axi_latency                       -     @502             
          wr_latency         integral                          32    'h0              
          rd_latency         integral                          32    'h0              
// Inter-Transaction Progress: Completed Transaction / Total Transaction
// Intra-Transaction Progress: Measured Latency / Latency Estimation * 100%
  M=16     K=288  N=1    PASS  max|err|=1.50e-06
  M=8      K=768  N=1    PASS  max|err|=2.04e-06
  M=4      K=288  N=40   PASS  max|err|=1.98e-06
=== ALL PASS ===
INFO: [COSIM 212-1000] *** C/RTL co-simulation finished: PASS ***
INFO: [HLS 200-112] Total CPU user time: 1110.34 seconds. Total CPU system time: 9.71 seconds. Total elapsed time: 793.04 seconds; peak allocated memory: 528.504 MB.
INFO: [vitis-run 60-791] Total elapsed time: 0h 13m 17s
[cs2401.02@rce ~]$ 