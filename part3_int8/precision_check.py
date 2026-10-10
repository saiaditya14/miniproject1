import numpy as np

rng = np.random.default_rng(0)


def run(M, K, N):
    A = rng.uniform(-1, 1, (M, K)).astype(np.float32)
    B = rng.uniform(-1, 1, (K, N)).astype(np.float32)
    ref = A.astype(np.float64) @ B.astype(np.float64)
    out = {}
    out["fp16"] = np.abs(A.astype(np.float16).astype(np.float64) @ B.astype(np.float64) - ref).max()
    sc = np.abs(A).reshape(M, -1, 32).max(2, keepdims=True) / 127.0
    q = np.round(A.reshape(M, -1, 32) / sc) * sc
    out["int8_gs32"] = np.abs(q.reshape(M, K).astype(np.float64) @ B.astype(np.float64) - ref).max()
    bf = (A.view(np.uint32) & 0xFFFF0000).view(np.float32)
    out["bf16_trunc"] = np.abs(bf.astype(np.float64) @ B.astype(np.float64) - ref).max()
    return out


for M, K, N in [(288, 288, 1), (768, 288, 32), (288, 768, 32), (4000, 288, 32), (768, 768, 768)]:
    o = run(M, K, N)
    print(f"{M}x{K}x{N}: " + "  ".join(f"{k} {v:.2e}" for k, v in o.items()))
print("grader tolerance: 1e-2 absolute")
