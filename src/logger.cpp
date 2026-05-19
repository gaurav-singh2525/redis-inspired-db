#include "logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>

using namespace std;

mutex coutMutex;

void logMessage(const string &msg)
{
    auto now =
        chrono::system_clock::now();

    time_t currentTime =
        chrono::system_clock::to_time_t(now);

    tm localTime{};

    localtime_r(
        &currentTime,
        &localTime);

    lock_guard<mutex> lock(coutMutex);

    cout << "["
         << put_time(
                &localTime,
                "%Y-%m-%d %H:%M:%S")
         << "] "
         << msg
         << endl;
}