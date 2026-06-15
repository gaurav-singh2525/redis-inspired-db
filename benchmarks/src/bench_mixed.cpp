#include "bench_suites.hpp"

#include "bench_client.hpp"
#include "bench_stats.hpp"

#include <iostream>
#include <sstream>

namespace bench
{

void runMixedBenchmark(const Config &cfg)
{
    printHeader(
        "Benchmark 3 — Mixed Workload (70% GET / 20% SET / 10% DEL)");

    constexpr std::size_t kOps = 100'000;

    Client client(cfg.host, cfg.port);
    client.connect();
    client.command("CLEAR");

    const std::size_t keyCount =
        effectiveKeyCount(client, 10'000);

    populateKeys(client, keyCount, "mixkey");

    std::cout
        << "Working key set: "
        << keyCount << " keys\n";

    const BenchResult result =
        measureWithLatency(
            "Mixed x100k",
            kOps,
            [&](std::size_t i)
            {
                const int bucket =
                    static_cast<int>(i % 10);

                std::ostringstream cmd;
                std::string response;

                if (bucket <= 6)
                {
                    cmd << "GET mixkey"
                        << (i % keyCount);
                    response = client.command(cmd.str());
                }
                else if (bucket <= 8)
                {
                    cmd << "SET mixkey"
                        << (i % keyCount)
                        << " mixed" << i;
                    response = client.command(cmd.str());

                    if (response != "OK")
                    {
                        throw std::runtime_error(
                            "SET failed in mixed workload");
                    }
                }
                else
                {
                    cmd << "DEL mixkey"
                        << (i % keyCount);
                    response = client.command(cmd.str());
                }
            });

    printResult(result);
    client.close();
}

} // namespace bench
