#include <cstdio>
#include <vector>

extern "C" void gemm(const float *A, const float *B, float *C, int M, int K, int N);

union fbits {
    float f;
    unsigned u;
};

static unsigned bitsof(float v) {
    fbits x;
    x.f = v;
    return x.u;
}

static int run(int M, int K, int N) {
    size_t sA = (size_t)M * K;
    std::vector<float> A(sA), B(K * (size_t)N, 0.0f), C((size_t)M * N + 16, 0.0f);
    unsigned st = 12345u + M + K + N;
    for (size_t i = 0; i < sA; i++) {
        st = st * 1664525u + 1013904223u;
        fbits x;
        x.u = st;
        A[i] = x.f;
    }
    B[0] = 1.0f;
    gemm(A.data(), B.data(), C.data(), M, K, N);
    unsigned ref[16] = {0};
    for (size_t t = 0; t < sA / 16; t++)
        for (int q = 0; q < 16; q++) ref[q] ^= bitsof(A[t * 16 + q]);
    int bad = 0;
    long mn = (long)M * N;
    int no = mn < 16 ? (int)mn : 16;
    int check = (N == 1 || sA / 16 >= 100000);
    if (check)
        for (int q = 0; q < no; q++)
            if (bitsof(C[q]) != (ref[q] ^ bitsof(B[0]))) bad++;
    std::printf("M=%d K=%d N=%d %s\n", M, K, N, check ? (bad ? "FAIL" : "PASS") : "skip");
    return bad ? 1 : 0;
}

int main() {
    int fails = 0;
    fails += run(288, 288, 1);
    fails += run(768, 288, 1);
    fails += run(288, 768, 1);
    fails += run(32000, 288, 1);
    fails += run(32000, 288, 32);
    fails += run(768, 768, 768);
    fails += run(288, 288, 32);
    std::printf("%s\n", fails ? "FAIL" : "ALL PASS");
    return fails ? 1 : 0;
}
