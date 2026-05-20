#include "persistence.h"
#include "wal.h"
#include "server.h"
#include "ttl_cleanup.h"

#include <thread>

int main()
{
    loadDatabase();
    replayWal();
    saveDatabase();
    clearWal();
    std::thread ttlThread(
        cleanupExpiredKeys);

    ttlThread.detach();
    startServer();
    return 0;
}