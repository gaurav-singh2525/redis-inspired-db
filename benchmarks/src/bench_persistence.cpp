#include "bench_suites.hpp"

#include "bench_client.hpp"
#include "bench_server.hpp"
#include "bench_stats.hpp"

#include <iostream>
#include <stdexcept>

namespace bench
{

void runPersistenceBenchmark(const Config &cfg)
{
    printHeader("Benchmark 5 — Persistence (Snapshot Save + Startup Recovery)");

    const std::size_t sizes[] = {
        1'000,
        10'000,
        100'000};

    std::cout
        << "Note: stops any running db_server, then restarts with BENCH_TIMING=1.\n"
        << "Do not run a manual ./db_server in another terminal.\n"
        << "Timing file: " << cfg.timingFile << "\n\n";

    stopServer();

    if (!waitForServer(cfg.host, cfg.port, 2000))
    {
        startServerWithTiming(cfg.projectRoot);

        if (!waitForServer(cfg.host, cfg.port, 30000))
        {
            throw std::runtime_error(
                "db_server failed to start — see /tmp/db_server_bench.log");
        }
    }
    else
    {
        std::cout
            << "Warning: server already listening on port "
            << cfg.port
            << ". Stopping it now...\n";

        stopServer();

        if (waitForServer(cfg.host, cfg.port, 2000))
        {
            throw std::runtime_error(
                "could not stop existing db_server on port " +
                std::to_string(cfg.port));
        }

        startServerWithTiming(cfg.projectRoot);

        if (!waitForServer(cfg.host, cfg.port, 30000))
        {
            throw std::runtime_error(
                "db_server failed to start — see /tmp/db_server_bench.log");
        }
    }

    printTableRow({
        "Keys",
        "Recovery (ms)",
        "Save (ms)"});

    for (std::size_t count : sizes)
    {
        Client client(cfg.host, cfg.port);
        client.connect();
        client.command("CLEAR");
        populateKeys(client, count, "persistkey");
        client.close();

        stopServer();

        if (waitForServer(cfg.host, cfg.port, 2000))
        {
            throw std::runtime_error(
                "db_server still running after stop");
        }

        clearTimingFile(cfg.timingFile);
        startServerWithTiming(cfg.projectRoot);

        const int timingTimeoutMs =
            count >= 100'000 ? 600'000 : 120'000;

        if (!waitForServer(cfg.host, cfg.port, timingTimeoutMs))
        {
            throw std::runtime_error(
                "db_server did not become ready during recovery");
        }

        const TimingMetrics metrics =
            waitForTimingMetrics(
                cfg.timingFile,
                timingTimeoutMs);

        if (metrics.recoveryMs < 0 ||
            metrics.saveMs < 0)
        {
            std::cerr
                << "Warning: missing timing data for "
                << count
                << " keys. Check "
                << cfg.timingFile
                << " and /tmp/db_server_bench.log\n";
        }

        printTableRow({
            std::to_string(count),
            metrics.recoveryMs >= 0
                ? std::to_string(metrics.recoveryMs)
                : "n/a",
            metrics.saveMs >= 0
                ? std::to_string(metrics.saveMs)
                : "n/a"});
    }

    std::cout
        << "\nRecovery = loadDatabase + replayWal\n"
        << "Save     = saveDatabase checkpoint\n"
        << "Server left running after benchmark.\n";
}

} // namespace bench
