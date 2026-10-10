import json
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

N = [1, 2, 4, 8, 16, 32, 64]
tok = [296, 263, 447, 729, 782, 510, 681]
fig, ax = plt.subplots(figsize=(6, 3.6))
ax.plot(N, tok, "o-")
ax.axhline(117, color="gray", linestyle="--", label="stock llama2.c fp32, N=1 (117 tok/s)")
ax.set_xscale("log", base=2)
ax.set_xlabel("batch size N (independent streams)")
ax.set_ylabel("aggregate tokens/s (CPU, laptop)")
ax.set_title("tok/s versus N: about 2.5x up to N=16, dip at N=32 (cause unverified)", fontsize=9)
ax.legend()
fig.tight_layout()
fig.savefig("figures/toks_vs_N.png", dpi=150)

d = json.load(open("part3/results/sweep.json"))
rows = [r for r in d["rows"] if r["gs"] == 32]
bits = [r["bits"] for r in rows]
mem = [r["weights_MB"] for r in rows]
colors = ["#55a868" if b >= 3 else "#c44e52" for b in bits]
fig, ax = plt.subplots(figsize=(6, 3.6))
ax.bar([str(b) for b in bits], mem, color=colors)
for i, m in enumerate(mem):
    ax.text(i, m + 0.3, f"{m:.1f}", ha="center", fontsize=8)
ax.axhline(60.8, color="gray", linestyle="--")
ax.text(5.4, 56.5, "fp32: 60.8 MB", ha="right", fontsize=8, color="gray")
ax.set_ylim(0, 66)
ax.set_xlabel("weight bits (group size 32)")
ax.set_ylabel("weights, MB")
ax.set_title("memory versus precision (green: story coherent, red: broken)", fontsize=9)
fig.text(0.5, 0.005, "coherence judged by reading the generated stories (part3/results/sweep.json)", ha="center", fontsize=7)
fig.tight_layout(rect=(0, 0.03, 1, 1))
fig.savefig("figures/precision_tradeoff.png", dpi=150)
print("ok")
