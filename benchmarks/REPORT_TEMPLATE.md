# Mini-Redis Benchmark Report

**Date:** 2026-JUN-15

**Machine:**

- CPU: Intel i5-12450H (12 Threads)
- RAM: 12 GB
- OS: Ubuntu/Linux
- Compiler: g++ (C++17)

**Server config:** `configs/server.conf`

---

## Benchmark 1 — Single Client SET Throughput

| Operations | Elapsed (s) | Throughput (ops/sec) | Avg Latency (µs) |
| ---------- | ----------- | -------------------- | ---------------- |
| 10,000     | 0.51        | 19452.43             | 51.41 us/op      |
| 50,000     | 2.82        | 17701.12             | 56.49 us/op      |
| 100,000    | 4.76        | 21000.27             | 47.62 us/op      |

---

## Benchmark 2 — Single Client GET Throughput

| Operations | Elapsed (s) | Throughput (ops/sec) | Avg Latency (µs) |
| ---------- | ----------- | -------------------- | ---------------- |
| 100,000    | 2.88        | 34748.98             | 28.78 us/op      |

---

## Benchmark 3 — Mixed Workload (70/20/10)

| Operations | Elapsed (s) | Throughput (ops/sec) | Avg Latency (µs) |
| ---------- | ----------- | -------------------- | ---------------- |
| 100,000    | 3.71        | 26948.00             | 37.11 us/op      |

---

## Benchmark 4 — Concurrent Clients

| Clients | Throughput (ops/sec) | Avg Latency (µs) |
| ------- | -------------------- | ---------------- |
| 1       | 26934                | 35 us/op         |
| 2       | 39669                | 48 us/op         |
| 4       | 50833                | 75 us/op         |
| 8       | 80928                | 96 us/op         |
| 16      | 95488                | 124 us/op        |

**Notes:** Server uses a fixed pool of **8 workers**. Throughput scales with increasing client concurrency until the 8-worker pool becomes saturated, after which gains diminish.

---

## Benchmark 5 — Persistence

| Keys    | Recovery (ms) | Save (ms) |
| ------- | ------------- | --------- |
| 1,000   | 3             | 0         |
| 10,000  | 18            | 1         |
| 100,000 | 170           | 14        |

Recovery = loadDatabase + replayWal
Save = saveDatabase checkpoint


---

## Benchmark 6 — LRU Stress

| Metric                      | Value    |
| --------------------------- | -------- |
| Cache capacity              | 10       |
| Insertions attempted        | 50,000   |
| Final key count             | 10       |
| Evictions                   | 49990    |
| Insert throughput (ops/sec) | 48632.50 |
| Correctness                 | PASS     |

---

## Benchmark 7 — TTL Stress

| Metric                  | Value |
| ----------------------- | ----- |
| Keys created            | 5,000 |
| TTL (seconds)           | 2     |
| COUNT before expiry     | 5000  |
| COUNT after expiry      | 0     |
| NULL reads after expiry | 5000  |
| Correctness             | PASS  |

---

## Observations

- GET operations achieved higher throughput than SET operations because reads avoid write-path overhead and persistence-related work.

- Throughput increased with the number of concurrent clients, demonstrating effective utilization of the worker pool. Scaling showed diminishing returns as worker threads became saturated and mutex contention increased.

- Snapshot persistence scaled well up to 100,000 keys, with recovery completing in under 200 ms and checkpoint creation in under 15 ms on the test machine.

- LRU eviction maintained cache capacity limits correctly under sustained insertion workloads.

- TTL expiration successfully removed expired keys through the background cleanup thread, with all test keys expiring as expected.
