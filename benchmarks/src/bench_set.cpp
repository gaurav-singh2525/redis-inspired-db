#include "bench_suites.hpp"

#include "bench_client.hpp"
#include "bench_stats.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace bench
{

void runSetBenchmark(const Config &cfg)
{
    printHeader("Benchmark 1 — Single Client SET Throughput");

    const std::size_t sizes[] = {
        10'000,
        50'000,
        100'000};

    for (std::size_t count : sizes)
    {
        Client client(cfg.host, cfg.port);
        client.connect();
        client.command("CLEAR");

        std::ostringstream label;
        label << "SET x" << count;

        const BenchResult result =
            measureWithLatency(
                label.str(),
                count,
                [&](std::size_t i)
                {
                    std::ostringstream cmd;
                    cmd << "SET setkey" << i
                        << " payload" << i;
                    const std::string response =
                        client.command(cmd.str());

                    if (response != "OK")
                    {
                        throw std::runtime_error(
                            "unexpected SET response: " +
                            response);
                    }
                });

        printResult(result);
        client.close();
    }
}

} // namespace bench
