#pragma once

#include <mutex>
#include <unordered_map>
#include <string>
#include <chrono>
#include <list>
#include <condition_variable>
#include <queue>

extern std::unordered_map<std::string, std::string> database;

extern std::mutex dbMutex;

extern std::unordered_map<
    std::string,
    std::chrono::system_clock::time_point>
    expiryMap;

extern std::list<std::string> lruList;

extern std::unordered_map<
    std::string,
    std::list<std::string>::iterator>
    lruMap;

extern size_t MAX_CAPACITY;

extern std::queue<std::pair<int, int>> clientQueue;

extern std::mutex queueMutex;

extern std::condition_variable cv;

extern int PORT;
extern int CACHE_CAPACITY;

extern std::string WAL_PATH;
extern std::string DB_PATH;
