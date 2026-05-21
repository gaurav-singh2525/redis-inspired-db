#include "lru.h"
#include "global.h"
#include "wal.h"

using namespace std;

void touchKey(
    const string &key)
{
    auto it =
        lruMap.find(key);

    if (it == lruMap.end())
        return;

    lruList.erase(
        it->second);

    lruList.push_front(
        key);

    lruMap[key] =
        lruList.begin();
}

void insertKey(
    const string &key)
{
    auto it =
        lruMap.find(key);

    if (it != lruMap.end())
    {
        touchKey(key);
        return;
    }

    lruList.push_front(
        key);

    lruMap[key] =
        lruList.begin();

    evictIfNeeded();
}

void removeKey(
    const string &key)
{
    auto it =
        lruMap.find(key);

    if (it == lruMap.end())
        return;

    lruList.erase(
        it->second);

    lruMap.erase(
        it);
}

void evictIfNeeded()
{
    while (
        database.size() >
            MAX_CAPACITY &&
        !lruList.empty())
    {
        string evictKey =
            lruList.back();

        lruList.pop_back();

        lruMap.erase(
            evictKey);

        expiryMap.erase(
            evictKey);

        if (database.erase(evictKey) > 0)
            appendToWal("DEL " + evictKey);
    }
}

string formatLruList()
{
    string result;

    for (const auto &key : lruList)
    {
        if (!result.empty())
            result += '\n';

        result += key;
    }

    if (result.empty())
        return "EMPTY";

    return result;
}