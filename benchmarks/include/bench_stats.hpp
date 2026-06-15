#pragma once

#include <chrono>
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace bench
{

using Clock = std::chrono::high_resolution_clock;

struct BenchResult
{
    std::string name;
    std::size_t operations = 0;
    double elapsedSec = 0.0;
    std::string extra;

    double throughputOpsPerSec() const;
    double avgLatencyMicros() const;
};

struct LatencyTracker
{
    std::vector<double> samplesUs;

    void addSample(double micros);
    double averageMicros() const;
};

BenchResult measure(
    const std::string &name,
    std::size_t operations,
    const std::function<void()> &fn);

BenchResult measureWithLatency(
    const std::string &name,
    std::size_t operations,
    const std::function<void(std::size_t)> &perOp);

void printResult(const BenchResult &result);
void printHeader(const std::string &title);
void printTableRow(
    const std::vector<std::string> &cols,
    int width = 16);

} // namespace bench
