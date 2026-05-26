#include "persistence.h"
#include "wal.h"
#include "server.h"
#include "ttl_cleanup.h"
#include "worker.h"

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

    for (int i = 0; i < 8; i++)
    {
        std::thread(worker).detach();
    }
    
    startServer();
    return 0;
}