#pragma once

#include <string>

namespace bench
{

struct TimingMetrics
{
    long long recoveryMs = -1;
    long long saveMs = -1;
    long long loadMs = -1;
    long long replayMs = -1;
    std::size_t keys = 0;
};

void stopServer();
void startServerWithTiming(
    const std::string &projectRoot);
TimingMetrics readTimingFile(
    const std::string &timingFile);
void clearTimingFile(
    const std::string &timingFile);
bool waitForServer(
    const std::string &host,
    int port,
    int timeoutMs);
TimingMetrics waitForTimingMetrics(
    const std::string &timingFile,
    int timeoutMs);

} // namespace bench
