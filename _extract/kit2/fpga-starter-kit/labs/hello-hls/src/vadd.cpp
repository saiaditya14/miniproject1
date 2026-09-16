// vadd — the "hello world" HLS kernel:  out[i] = in1[i] + in2[i]
//
// Three DRAM-backed arrays exposed as AXI-master (m_axi) ports and one
// pipelined loop. `size` rides on the auto-generated s_axilite control
// interface. This is the smoke-test kernel for the RCE starter kit — the
// smallest design that still exercises csim, csynth, cosim, and a v++ build.

#include <cstdint>

// Upper bound for the m_axi depth= hints (needed so C/RTL co-sim knows how many
// elements each pointer may touch). The kernel itself is size-agnostic.
#define MAX_SIZE 4096

extern "C" {
void vadd(const unsigned int *in1,  // read-only input vector 1
          const unsigned int *in2,  // read-only input vector 2
          unsigned int *out,        // output vector
          int size) {               // number of elements
#pragma HLS INTERFACE m_axi port = in1 bundle = gmem0 depth = MAX_SIZE
#pragma HLS INTERFACE m_axi port = in2 bundle = gmem1 depth = MAX_SIZE
#pragma HLS INTERFACE m_axi port = out bundle = gmem0 depth = MAX_SIZE

vadd_loop:
    for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE II = 1
        out[i] = in1[i] + in2[i];
    }
}
}
