#include "bench_stats.hpp"

namespace bench
{

double BenchResult::throughputOpsPerSec() const
{
    if (elapsedSec <= 0.0)
        return 0.0;

    return static_cast<double>(operations) /
           elapsedSec;
}

double BenchResult::avgLatencyMicros() const
{
    if (operations == 0)
        return 0.0;

    return (elapsedSec * 1'000'000.0) /
           static_cast<double>(operations);
}

void LatencyTracker::addSample(double micros)
{
    samplesUs.push_back(micros);
}

double LatencyTracker::averageMicros() const
{
    if (samplesUs.empty())
        return 0.0;

    double sum = 0.0;

    for (double sample : samplesUs)
        sum += sample;

    return sum /
           static_cast<double>(samplesUs.size());
}

BenchResult measure(
    const std::string &name,
    std::size_t operations,
    const std::function<void()> &fn)
{
    const auto start = Clock::now();
    fn();
    const auto end = Clock::now();

    BenchResult result;
    result.name = name;
    result.operations = operations;
    result.elapsedSec =
        std::chrono::duration<double>(end - start)
            .count();

    return result;
}

BenchResult measureWithLatency(
    const std::string &name,
    std::size_t operations,
    const std::function<void(std::size_t)> &perOp)
{
    LatencyTracker tracker;
    const auto start = Clock::now();

    for (std::size_t i = 0; i < operations; ++i)
    {
        const auto opStart = Clock::now();
        perOp(i);
        const auto opEnd = Clock::now();

        tracker.addSample(
            std::chrono::duration<double,
                                  std::micro>(
                opEnd - opStart)
                .count());
    }

    const auto end = Clock::now();

    BenchResult result;
    result.name = name;
    result.operations = operations;
    result.elapsedSec =
        std::chrono::duration<double>(end - start)
            .count();
    result.extra =
        "latency_tracker_avg_us=" +
        std::to_string(
            static_cast<long long>(
                tracker.averageMicros()));

    return result;
}

void printHeader(const std::string &title)
{
    std::cout
        << "\n========================================\n"
        << title << '\n'
        << "========================================\n";
}

void printResult(const BenchResult &result)
{
    std::cout << std::fixed
              << std::setprecision(2);

    std::cout << "Benchmark Name : "
              << result.name << '\n';
    std::cout << "Operations     : "
              << result.operations << '\n';
    std::cout << "Elapsed Time   : "
              << result.elapsedSec << " s\n";
    std::cout << "Throughput     : "
              << result.throughputOpsPerSec()
              << " ops/sec\n";
    std::cout << "Average Latency: "
              << result.avgLatencyMicros()
              << " us/op\n";

    if (!result.extra.empty())
        std::cout << "Notes          : "
                  << result.extra << '\n';
}

void printTableRow(
    const std::vector<std::string> &cols,
    int width)
{
    for (const auto &col : cols)
    {
        std::cout << std::left
                  << std::setw(width)
                  << col;
    }

    std::cout << '\n';
}

} // namespace bench
