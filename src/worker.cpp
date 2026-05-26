#include "global.h"
#include "client_handler.h"
#include "worker.h"

using namespace std;

void worker()
{
    while (true)
    {
        unique_lock<mutex>
            lock(queueMutex);

        cv.wait(lock, []
                {
                    return !clientQueue.empty();
                });

        auto client =
            clientQueue.front();

        clientQueue.pop();

        lock.unlock();

        int client_fd =
            client.first;

        int clientID =
            client.second;

        handleClient(
            client_fd,
            clientID);
    }
}