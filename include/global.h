#pragma once

#include <mutex>
#include <unordered_map>
#include <string>
#include <chrono>

extern std::unordered_map<std::string, std::string> database;

extern std::mutex dbMutex;

extern std::unordered_map<
    std::string,
    std::chrono::system_clock::time_point>
    expiryMap;