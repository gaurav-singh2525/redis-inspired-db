#include "persistence.h"
#include "wal.h"
#include "server.h"


int main()
{
    loadDatabase();
    replayWal();
    saveDatabase();
    clearWal();
    startServer();
    return 0;
}