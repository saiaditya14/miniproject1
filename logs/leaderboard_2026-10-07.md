# GEMM leaderboard snapshot, 2026-10-07 (from the portal screenshot, sorted by square)

Our roll: 2023101096. Our run is v3 (submission id 58, 279.1 MHz kernel clock).

| rank | roll | square 768^3 (ms) | N=1 total (ms) | N=32 total (ms) | tok/s |
|---|---|---|---|---|---|
| 1 | 2023111030 | 1.166 | 3.232 | 3.455 | 118.11 |
| 2 | 2023112003 | 5.052 | 3.645 | 3.625 | 104.12 |
| 3 | 2023111019 | 6.451 | 3.212 | 4.384 | 118.72 |
| 4 | 2025121012 | 7.463 | 3.242 | 5.629 | 115.38 |
| 5 | 2024112019 | 12.250 | 10.043 | 14.869 | 50.58 |
| 6 | 2023112005 | 12.395 | 3.758 | 8.648 | 104.25 |
| 7 | 2025201015 | 13.484 | 6.480 | 60.913 | 74.26 |
| 8 | 2024101002 | 14.967 | 39.425 | 9.347 | 14.59 |
| 9 | 2025121003 | 25.424 | 16.652 | 33.771 | 31.44 |
| **10** | **2023101096 (us)** | **32.701** | **8.805** | **24.026** | **57.24** |
| 11 | 2026203010 | 159.469 | 111.895 | 169.120 | 5.44 |
| 12 | 2024112022 | 1514.686 | 88.123 | 1099.446 | 6.84 |
| 13 | 2024101012 | 1545.849 | 36.786 | 1657.617 | 15.41 |
| 14 | 2023111003 | 13435.152 | 295.749 | 9446.248 | 2.12 |

## Our position per column (14 rows)
- square: 10th. N=1 sum: 7th. N=32 sum: 8th. tok/s: 7th.
- N=1 beats rows 5 (10.04), 8 (39.4), 9 (16.7) and everything below; N=32 beats rows 7 (60.9), 9 (33.8); tok/s beats row 5 (50.58) and everything below.

## Derived (square = 906 MFLOP, N=32 sum = 624 MFLOP, N=1 sum moves 38.97 MB of A)
- Rank 1 square = 777 GFLOP/s; rank 2 = 179; rank 3 = 140; rank 4 = 121; ours = 27.7.
- Top-4 N=1 cluster at 3.21-3.65 ms = 10.7-12.1 GB/s of A (60-68% of one 19.2 GB/s port); ours 8.805 ms = 4.4 GB/s.
- Top-4 tok/s is 104-119; the cluster at about 118 matches N=1 about 3.2 ms and N=32 3.5-4.4 ms.

## Targets read off this table
- Top 3 needs: square <= 6.45 ms (140 GFLOP/s), N=1 <= about 3.6 ms, N=32 <= about 4.4 ms, tok/s >= about 115.
- Rank 2 on square needs 5.05 ms (179 GFLOP/s).
