#pragma once

#include <mutex>
#include <unordered_map>
#include <string>
#include <chrono>
#include <list>

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

extern const size_t MAX_CAPACITY;