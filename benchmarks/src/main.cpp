#include "bench_config.hpp"
#include "bench_suites.hpp"
#include "bench_stats.hpp"

#include <cstring>
#include <iostream>
#include <string>

namespace
{

void printUsage(const char *program)
{
    std::cout
        << "Usage: " << program
        << " <suite>\n\n"
        << "Suites:\n"
        << "  all\n"
        << "  set\n"
        << "  get\n"
        << "  mixed\n"
        << "  concurrent\n"
        << "  persistence\n"
        << "  lru\n"
        << "  ttl\n";
}

} // namespace

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printUsage(argv[0]);
        return 1;
    }

    const std::string suite = argv[1];
    const bench::Config cfg = bench::loadConfig();

    std::cout
        << "db_server benchmark runner\n"
        << "Target: " << cfg.host
        << ":" << cfg.port << "\n";

    try
    {
        if (suite == "all" || suite == "set")
            bench::runSetBenchmark(cfg);

        if (suite == "all" || suite == "get")
            bench::runGetBenchmark(cfg);

        if (suite == "all" || suite == "mixed")
            bench::runMixedBenchmark(cfg);

        if (suite == "all" || suite == "concurrent")
            bench::runConcurrentBenchmark(cfg);

        if (suite == "all" || suite == "persistence")
            bench::runPersistenceBenchmark(cfg);

        if (suite == "all" || suite == "lru")
            bench::runLruBenchmark(cfg);

        if (suite == "all" || suite == "ttl")
            bench::runTtlBenchmark(cfg);

        if (suite != "all" &&
            suite != "set" &&
            suite != "get" &&
            suite != "mixed" &&
            suite != "concurrent" &&
            suite != "persistence" &&
            suite != "lru" &&
            suite != "ttl")
        {
            printUsage(argv[0]);
            return 1;
        }
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Benchmark failed: "
                  << ex.what() << '\n';
        return 1;
    }

    return 0;
}
