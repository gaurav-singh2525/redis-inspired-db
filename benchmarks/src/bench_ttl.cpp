#include "bench_suites.hpp"

#include "bench_client.hpp"
#include "bench_stats.hpp"

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

namespace bench
{

void runTtlBenchmark(const Config &cfg)
{
    printHeader("Benchmark 7 — TTL Stress Test");

    constexpr std::size_t kKeys = 5'000;
    constexpr int kTtlSeconds = 2;

    Client client(cfg.host, cfg.port);
    client.connect();
    client.command("CLEAR");

    const auto populateStart = Clock::now();

    for (std::size_t i = 0; i < kKeys; ++i)
    {
        std::ostringstream setCmd;
        setCmd << "SET ttlkey" << i
               << " payload";

        if (client.command(setCmd.str()) != "OK")
        {
            throw std::runtime_error(
                "TTL SET failed");
        }

        std::ostringstream expireCmd;
        expireCmd << "EXPIRE ttlkey" << i
                  << " " << kTtlSeconds;

        if (client.command(expireCmd.str()) != "OK")
        {
            throw std::runtime_error(
                "TTL EXPIRE failed");
        }
    }

    const auto populateEnd = Clock::now();

    const std::string countBefore =
        client.command("COUNT");

    std::cout << "Keys created   : "
              << kKeys << '\n';
    std::cout << "COUNT before   : "
              << countBefore << '\n';

    const auto waitStart = Clock::now();
    std::this_thread::sleep_for(
        std::chrono::seconds(kTtlSeconds + 2));
    const auto waitEnd = Clock::now();

    const std::string countAfter =
        client.command("COUNT");

    std::size_t expiredReads = 0;

    for (std::size_t i = 0; i < kKeys; ++i)
    {
        std::ostringstream getCmd;
        getCmd << "GET ttlkey" << i;

        if (client.command(getCmd.str()) == "NULL")
            ++expiredReads;
    }

    BenchResult populateResult;
    populateResult.name = "TTL populate + EXPIRE";
    populateResult.operations = kKeys;
    populateResult.elapsedSec =
        std::chrono::duration<double>(
            populateEnd - populateStart)
            .count();

    printResult(populateResult);

    std::cout << "Cleanup wait   : "
              << std::chrono::duration<double>(
                     waitEnd - waitStart)
                     .count()
              << " s\n";
    std::cout << "COUNT after    : "
              << countAfter << '\n';
    std::cout << "NULL on GET    : "
              << expiredReads << '\n';

  const bool pass =
        countAfter == "0" &&
        expiredReads == kKeys;

    std::cout << "Correctness    : "
              << (pass ? "PASS" : "FAIL")
              << " (all keys expired and removed)\n";

    client.close();
}

} // namespace bench
