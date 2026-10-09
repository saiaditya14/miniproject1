import os, subprocess, sys, struct, json
import torch

L = sys.argv[1]
OUT = sys.argv[2]
os.makedirs(OUT, exist_ok=True)
sys.path.insert(0, L)
import export

PROMPTS = ["Once upon a time", "One day, a little boy", "The dog and the cat", "Lily wanted to", "There was a big"]
NTOK = 256
BITS = {"v": 8}


def quantize_bits(w, group_size):
    qmax = (1 << (BITS["v"] - 1)) - 1
    w = w.float().reshape(-1, group_size)
    wmax = torch.abs(w).max(dim=1).values
    scale = wmax / qmax
    scale = torch.where(scale == 0, torch.ones_like(scale), scale)
    q = torch.round(w / scale[:, None]).clamp(-qmax, qmax).to(torch.int8)
    err = torch.abs(q.float() * scale[:, None] - w).max(dim=1).values.max().item()
    return q, scale, err


export.quantize_q80 = quantize_bits


def story(exe, model, prompt):
    r = subprocess.run([exe, model, "-t", "0.0", "-s", "42", "-n", str(NTOK), "-i", prompt],
                       cwd=L, capture_output=True, text=True, encoding="utf-8", errors="replace")
    txt = r.stdout
    k = txt.rfind("achieved tok/s")
    return txt[:k].strip() if k >= 0 else txt.strip()


def prefix(a, b):
    n = 0
    for x, y in zip(a, b):
        if x != y:
            break
        n += 1
    return n


model = export.load_checkpoint(os.path.join(L, "stories15M.pt"))
ref = {p: story(os.path.join(L, "run.exe"), os.path.join(L, "stories15M.bin"), p) for p in PROMPTS}
n_elems = 32000 * 288 + 6 * (4 * 288 * 288 + 3 * 288 * 768)
rows = []
configs = [(8, 32), (8, 16), (8, 8), (6, 32), (5, 32), (4, 32), (4, 16), (4, 8), (3, 32), (3, 8), (2, 32), (2, 8)]
for bits, gs in configs:
    BITS["v"] = bits
    path = os.path.join(L, f"sweep_b{bits}_g{gs}.bin")
    with open(os.devnull, "w") as dn:
        old = sys.stdout
        sys.stdout = dn
        export.version2_export(model, path, group_size=gs)
        sys.stdout = old
    stories = {p: story(os.path.join(L, "runq.exe"), path, p) for p in PROMPTS}
    keep = [prefix(ref[p], stories[p]) / max(1, len(ref[p])) for p in PROMPTS]
    cls_mb = 32000 * 288 * (bits / 8 + 4 / gs) / 1e6
    tot_mb = n_elems * (bits / 8 + 4 / gs) / 1e6
    rows.append(dict(bits=bits, gs=gs, classifier_MB=round(cls_mb, 2), weights_MB=round(tot_mb, 2),
                     same_prefix_pct=round(100 * sum(keep) / len(keep), 1), story=stories[PROMPTS[0]]))
    print(bits, gs, round(cls_mb, 2), round(tot_mb, 2), round(100 * sum(keep) / len(keep), 1), flush=True)
    os.remove(path)
json.dump(dict(ref=ref[PROMPTS[0]], rows=rows), open(os.path.join(OUT, "sweep.json"), "w"), indent=1)
