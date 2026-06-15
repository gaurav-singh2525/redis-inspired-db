#include "bench_suites.hpp"

#include "bench_client.hpp"
#include "bench_stats.hpp"

#include <iostream>
#include <sstream>

namespace bench
{

void runLruBenchmark(const Config &cfg)
{
    printHeader("Benchmark 6 — LRU Stress Test");

    std::cout
        << "Requires small cache_capacity in configs/server.conf\n"
        << "(recommended: cache_capacity=10). Restart server after changing config.\n\n";

    constexpr std::size_t kInsertions = 50'000;

    Client client(cfg.host, cfg.port);
    client.connect();
    client.command("CLEAR");

    const std::string capacity =
        client.command("CAPACITY");

    std::size_t evictionCount = 0;
    const auto start = Clock::now();

    for (std::size_t i = 0; i < kInsertions; ++i)
    {
        std::ostringstream cmd;
        cmd << "SET lrukey" << i
            << " value" << i;

        const std::string response =
            client.command(cmd.str());

        if (response != "OK")
        {
            throw std::runtime_error(
                "LRU SET failed: " + response);
        }
    }

    const auto end = Clock::now();

    const std::string finalCount =
        client.command("COUNT");

    const std::size_t capacityValue =
        std::stoull(capacity);

    if (std::stoull(finalCount) > capacityValue)
    {
        throw std::runtime_error(
            "LRU correctness failed: COUNT exceeds CAPACITY");
    }

    if (kInsertions > capacityValue)
    {
        evictionCount =
            kInsertions - std::stoull(finalCount);
    }

    BenchResult result;
    result.name = "LRU stress insertions";
    result.operations = kInsertions;
    result.elapsedSec =
        std::chrono::duration<double>(end - start)
            .count();
    result.extra =
        "capacity=" + capacity +
        " final_count=" + finalCount +
        " evictions=" +
        std::to_string(evictionCount);

    printResult(result);

    std::cout << "Correctness    : PASS (COUNT <= CAPACITY)\n";
    client.close();
}

} // namespace bench
