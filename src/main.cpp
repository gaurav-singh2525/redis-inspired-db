#include "persistence.h"
#include "wal.h"
#include "server.h"
#include "ttl_cleanup.h"
#include "worker.h"
#include "config.h"
#include "global.h"

#include <thread>
#include <chrono>
#include <cstdlib>
#include <fstream>

int main()
{
    if (getenv("BENCH_TIMING"))
    {
        std::ofstream clearFile(
            "data/bench_timing.txt",
            std::ios::trunc);
    }

    loadConfig();

    auto recoveryStart =
        std::chrono::high_resolution_clock::now();

    loadDatabase();
    replayWal();

    auto recoveryEnd =
        std::chrono::high_resolution_clock::now();

    if (getenv("BENCH_TIMING"))
    {
        auto recoveryMs =
            std::chrono::duration_cast<
                std::chrono::milliseconds>(
                recoveryEnd - recoveryStart)
                .count();

        std::ofstream fout(
            "data/bench_timing.txt",
            std::ios::app);

        if (fout.is_open())
        {
            fout << "recovery_ms="
                 << recoveryMs
                 << " keys="
                 << database.size()
                 << "\n";
        }
    }

    saveDatabase();
    clearWal();
    std::thread ttlThread(
        cleanupExpiredKeys);

    ttlThread.detach();

    for (int i = 0; i < 8; i++)
    {
        std::thread(worker).detach();
    }

    startServer();
    return 0;
}