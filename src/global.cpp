#include "global.h"
#include <unordered_map>
#include <mutex>

using namespace std;

unordered_map<string, string>
    database;

mutex dbMutex;