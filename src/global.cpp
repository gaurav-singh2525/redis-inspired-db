#include "global.h"
#include <mutex>

using namespace std;

unordered_map<string, string>
    database;

mutex dbMutex;

unordered_map<
    string,
    chrono::system_clock::time_point>
    expiryMap;

std::list<std::string> lruList;

std::unordered_map<
    std::string,
    std::list<std::string>::iterator>
    lruMap;

const size_t MAX_CAPACITY = 10;