#include "bench_server.hpp"

#include "bench_client.hpp"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <thread>

namespace bench
{

void stopServer()
{
    std::system(
        "pkill -x db_server 2>/dev/null || "
        "pkill -f './db_server' 2>/dev/null || "
        "true");

    std::this_thread::sleep_for(
        std::chrono::milliseconds(800));
}

void startServerWithTiming(
    const std::string &projectRoot)
{
    const std::string cmd =
        "cd " + projectRoot +
        " && BENCH_TIMING=1 ./db_server"
        " > /tmp/db_server_bench.log 2>&1 &";

    std::system(cmd.c_str());
}

void clearTimingFile(
    const std::string &timingFile)
{
    std::ofstream file(
        timingFile,
        std::ios::trunc);
}

TimingMetrics readTimingFile(
    const std::string &timingFile)
{
    TimingMetrics metrics;
    std::ifstream fin(timingFile);

    if (!fin.is_open())
        return metrics;

    std::string line;

    while (std::getline(fin, line))
    {
        const auto eq = line.find('=');

        if (eq == std::string::npos)
            continue;

        const std::string key =
            line.substr(0, eq);

        std::string value =
            line.substr(eq + 1);

        const std::size_t keysPos =
            value.find(" keys=");

        if (keysPos != std::string::npos)
        {
            metrics.keys = std::stoull(
                value.substr(keysPos + 6));
            value = value.substr(0, keysPos);
        }

        const long long ms =
            std::stoll(value);

        if (key == "recovery_ms")
            metrics.recoveryMs = ms;
        else if (key == "save_ms")
            metrics.saveMs = ms;
        else if (key == "load_ms")
            metrics.loadMs = ms;
        else if (key == "replay_ms")
            metrics.replayMs = ms;
    }

    return metrics;
}

bool waitForServer(
    const std::string &host,
    int port,
    int timeoutMs)
{
    const auto deadline =
        std::chrono::steady_clock::now() +
        std::chrono::milliseconds(timeoutMs);

    while (std::chrono::steady_clock::now() <
           deadline)
    {
        try
        {
            Client client(host, port);
            client.connect();

            if (client.command("PING") == "PONG")
                return true;
        }
        catch (...)
        {
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(100));
    }

    return false;
}

TimingMetrics waitForTimingMetrics(
    const std::string &timingFile,
    int timeoutMs)
{
    const auto deadline =
        std::chrono::steady_clock::now() +
        std::chrono::milliseconds(timeoutMs);

    while (std::chrono::steady_clock::now() <
           deadline)
    {
        const TimingMetrics metrics =
            readTimingFile(timingFile);

        if (metrics.recoveryMs >= 0 &&
            metrics.saveMs >= 0)
        {
            return metrics;
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(200));
    }

    return readTimingFile(timingFile);
}

} // namespace bench
