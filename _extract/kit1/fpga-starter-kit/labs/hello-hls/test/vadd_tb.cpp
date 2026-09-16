// vadd_tb.cpp — C/RTL testbench for the vadd smoke test.
//
// Used by csim (compiled natively with the kernel) and by cosim (run against
// the synthesized RTL). Builds deterministic inputs, computes a golden
// reference, calls the kernel, compares. Return 0 = pass, non-zero = fail —
// that return code is how HLS decides csim/cosim PASS/FAIL.

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

extern "C" void vadd(const unsigned int *in1, const unsigned int *in2,
                     unsigned int *out, int size);

int main(int argc, char **argv) {
    // Keep default small so cosim stays quick; override with argv[1].
    int N = (argc > 1 && std::atoi(argv[1]) > 0) ? std::atoi(argv[1]) : 1024;
    std::cout << "vadd testbench: N=" << N << std::endl;

    std::vector<unsigned int> a(N), b(N), out(N, 0), golden(N);
    for (int i = 0; i < N; ++i) {
        a[i] = i * 3u + 1u;
        b[i] = i * 7u + 2u;
        golden[i] = a[i] + b[i];  // 32-bit wraparound matches hardware
    }

    vadd(a.data(), b.data(), out.data(), N);

    int errors = 0;
    for (int i = 0; i < N; ++i)
        if (out[i] != golden[i] && ++errors <= 10)
            std::cerr << "Mismatch at i=" << i << " expected=" << golden[i]
                      << " got=" << out[i] << std::endl;

    if (errors == 0) {
        std::cout << "TEST PASSED" << std::endl;
        return 0;
    }
    std::cerr << "TEST FAILED with " << errors << " mismatches" << std::endl;
    return 1;
}
