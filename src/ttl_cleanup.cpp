#include "global.h"

#include <thread>
#include <chrono>

using namespace std;

void cleanupExpiredKeys()
{
    while (true)
    {
        {
            lock_guard<mutex>
                lock(dbMutex);

            for (auto it =
                     expiryMap.begin();
                 it != expiryMap.end();)
            {
                if (
                    chrono::system_clock::now() > it->second)
                {
                    database.erase(
                        it->first);

                    it =
                        expiryMap.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        this_thread::sleep_for(
            chrono::seconds(1));
    }
}