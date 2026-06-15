#include "bench_suites.hpp"

#include "bench_client.hpp"
#include "bench_stats.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace bench
{

void runGetBenchmark(const Config &cfg)
{
    printHeader("Benchmark 2 — Single Client GET Throughput");

    constexpr std::size_t kDesiredKeys = 100'000;
    constexpr std::size_t kOps = 100'000;

    Client setup(cfg.host, cfg.port);
    setup.connect();
    setup.command("CLEAR");

    const std::size_t keyCount =
        effectiveKeyCount(setup, kDesiredKeys);

    populateKeys(setup, keyCount, "getkey");
    setup.close();

    std::cout
        << "Keys in store  : "
        << keyCount
        << " (limited by server CAPACITY / LRU)\n";

    Client client(cfg.host, cfg.port);
    client.connect();

    const BenchResult result =
        measureWithLatency(
            "GET x100k",
            kOps,
            [&](std::size_t i)
            {
                std::ostringstream cmd;
                cmd << "GET getkey"
                    << (i % keyCount);
                const std::string response =
                    client.command(cmd.str());

                if (response == "NULL")
                {
                    throw std::runtime_error(
                        "GET returned NULL for key getkey" +
                        std::to_string(i % keyCount));
                }
            });

    printResult(result);
    client.close();
}

} // namespace bench
