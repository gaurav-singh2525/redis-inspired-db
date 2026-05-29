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

list<string> lruList;

std::unordered_map<
    std::string,
    std::list<std::string>::iterator>
    lruMap;

queue<pair<int, int>>
    clientQueue;

mutex queueMutex;

condition_variable cv;

int PORT = 9000;
int CACHE_CAPACITY = 100;
const size_t MAX_CAPACITY = CACHE_CAPACITY;
string WAL_PATH = "data/wal.log";
string DB_PATH = "data/store.cdb";