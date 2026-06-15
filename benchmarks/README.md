# Mini_Redis Benchmarks

Professional benchmarking suite for **Mini-Redis**. All benchmark code lives here and is **completely separate** from the server implementation.

## Prerequisites

1. Build the server from the project root:
   ```bash
   make
   ```
2. Create the data directory:
   ```bash
   mkdir -p data
   ```
3. Build benchmarks:
   ```bash
   cd benchmarks
   make
   ```

## Running benchmarks

Start the server from the **project root** (in a separate terminal):

```bash
./db_server
```

Run benchmarks from the `benchmarks/` directory:

```bash
cd benchmarks
./bench_runner <suite>
```

### Suites

| Command | Description |
|---------|-------------|
| `./bench_runner set` | Single-client SET throughput (10k / 50k / 100k) |
| `./bench_runner get` | Single-client GET throughput (100k) |
| `./bench_runner mixed` | 70% GET / 20% SET / 10% DEL (100k) |
| `./bench_runner concurrent` | 1 / 2 / 4 / 8 / 16 clients |
| `./bench_runner persistence` | Snapshot save + startup recovery (1k / 10k / 100k) |
| `./bench_runner lru` | LRU stress + correctness |
| `./bench_runner ttl` | TTL expiry stress + correctness |
| `./bench_runner all` | Run every suite in order |

### Special setup

**Benchmark 6 (LRU):** set a small cache in `configs/server.conf`:

```ini
cache_capacity=10
```

Restart `db_server` before running `./bench_runner lru`.

**Benchmark 5 (Persistence):** stops and restarts `db_server` automatically with `BENCH_TIMING=1`. Do not run other clients against the server during this suite.


## Output

Each benchmark prints:

- Benchmark name
- Operations
- Elapsed time (seconds)
- Throughput (ops/sec)
- Average latency (µs/op)

