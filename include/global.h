#pragma once

#include <mutex>
#include <unordered_map>
#include <string>

extern std::unordered_map<std::string, std::string> database;

extern std::mutex dbMutex;