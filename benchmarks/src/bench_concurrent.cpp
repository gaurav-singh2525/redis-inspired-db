#include "bench_suites.hpp"

#include "bench_client.hpp"
#include "bench_stats.hpp"

#include <atomic>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

namespace bench
{

namespace
{

struct ConcurrentStats
{
    std::atomic<std::size_t> completed{0};
    std::vector<double> latencies;
    std::mutex latenciesMutex;
};

void clientWorker(
    const Config &cfg,
    std::size_t operationsPerClient,
    std::size_t keyCount,
    ConcurrentStats &stats)
{
    Client client(cfg.host, cfg.port);
    client.connect();

    for (std::size_t i = 0;
         i < operationsPerClient;
         ++i)
    {
        double latency = 0.0;
        std::ostringstream cmd;

        const int bucket =
            static_cast<int>(i % 10);

        if (bucket <= 6)
        {
            cmd << "GET mixkey"
                << (i % keyCount);
            client.commandTimed(cmd.str(), latency);
        }
        else if (bucket <= 8)
        {
            cmd << "SET mixkey"
                << (i % keyCount)
                << " v" << i;
            client.commandTimed(cmd.str(), latency);
        }
        else
        {
            cmd << "DEL mixkey"
                << (i % keyCount);
            client.commandTimed(cmd.str(), latency);
        }

        {
            std::lock_guard<std::mutex> lock(
                stats.latenciesMutex);
            stats.latencies.push_back(latency);
        }

        stats.completed.fetch_add(
            1,
            std::memory_order_relaxed);
    }

    client.close();
}

} // namespace

void runConcurrentBenchmark(const Config &cfg)
{
    printHeader("Benchmark 4 — Concurrent Client Throughput");

    const int clientCounts[] = {
        1, 2, 4, 8, 16};

    constexpr std::size_t kOpsPerClient = 10'000;

    Client setup(cfg.host, cfg.port);
    setup.connect();
    setup.command("CLEAR");

    const std::size_t keyCount =
        effectiveKeyCount(setup, 1'000);

    populateKeys(setup, keyCount, "mixkey");
    setup.close();

    std::cout
        << "Working key set: "
        << keyCount << " keys\n";

    printTableRow({
        "Clients",
        "Throughput",
        "Avg Latency"});

    for (int clients : clientCounts)
    {
        ConcurrentStats stats;
        const auto start = Clock::now();
        std::vector<std::thread> threads;

        for (int c = 0; c < clients; ++c)
        {
            threads.emplace_back(
                clientWorker,
                std::cref(cfg),
                kOpsPerClient,
                keyCount,
                std::ref(stats));
        }

        for (auto &thread : threads)
            thread.join();

        const auto end = Clock::now();

        const double elapsed =
            std::chrono::duration<double>(
                end - start)
                .count();

        const std::size_t totalOps =
            static_cast<std::size_t>(clients) *
            kOpsPerClient;

        double latencySum = 0.0;

        for (double sample : stats.latencies)
            latencySum += sample;

        const double avgLatency =
            stats.latencies.empty()
                ? 0.0
                : latencySum /
                      static_cast<double>(
                          stats.latencies.size());

        const double throughput =
            elapsed > 0.0
                ? static_cast<double>(totalOps) /
                      elapsed
                : 0.0;

        printTableRow({
            std::to_string(clients),
            std::to_string(
                static_cast<long long>(throughput)) +
                " ops/sec",
            std::to_string(
                static_cast<long long>(avgLatency)) +
                " us/op"});
    }
}

} // namespace bench
