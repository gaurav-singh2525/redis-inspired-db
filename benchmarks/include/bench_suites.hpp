#pragma once

#include "bench_config.hpp"

namespace bench
{

void runSetBenchmark(const Config &cfg);
void runGetBenchmark(const Config &cfg);
void runMixedBenchmark(const Config &cfg);
void runConcurrentBenchmark(const Config &cfg);
void runPersistenceBenchmark(const Config &cfg);
void runLruBenchmark(const Config &cfg);
void runTtlBenchmark(const Config &cfg);

} // namespace bench
