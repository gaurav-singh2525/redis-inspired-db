#include "persistence.h"
#include "global.h"
#include "parser.h"
#include "lru.h"

#include <fstream>
#include <iostream>
#include <mutex>
#include <cctype>
#include <algorithm>
#include <chrono>
#include <cstdlib>

using namespace std;

static void writeBenchTiming(
    const string &metric,
    long long ms,
    size_t keys = 0)
{
    if (!getenv("BENCH_TIMING"))
        return;

    ofstream fout(
        "data/bench_timing.txt",
        ios::app);

    if (!fout.is_open())
        return;

    fout << metric << "=" << ms;

    if (keys > 0)
        fout << " keys=" << keys;

    fout << "\n";
}

void saveDatabase()
{
    auto start =
        chrono::high_resolution_clock::now();

    lock_guard<mutex> lock(dbMutex);

    ofstream fout(DB_PATH);

    if (!fout.is_open())
    {
        cerr << "Failed to open file\n";
        return;
    }

    for (const auto &[key, value] : database)
    {
        fout << key << "=" << value << "\n";
    }

    auto end =
        chrono::high_resolution_clock::now();

    writeBenchTiming(
        "save_ms",
        chrono::duration_cast<
            chrono::milliseconds>(
            end - start)
            .count(),
        database.size());

    cout << "saved success\n";
}

void clearSnapshot()
{
    ofstream file(DB_PATH, ios::trunc);
}

void loadDatabase()
{
    auto start =
        chrono::high_resolution_clock::now();

    lock_guard<mutex> lock(dbMutex);

    ifstream fin(DB_PATH);

    if (!fin.is_open())
    {
        writeBenchTiming("load_ms", 0, 0);
        return;
    }

    string line;

    while (getline(fin, line))
    {
        size_t pos = line.find('=');

        // no '='
        if (pos == string::npos)
            continue;

        // more than one '='
        if (line.find('=', pos + 1) != string::npos)
            continue;

        // empty key
        if (pos == 0)
            continue;

        // empty value
        if (pos == line.size() - 1)
            continue;

        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);

        database[key] = value;
        insertKey(key);
    }

    auto end =
        chrono::high_resolution_clock::now();

    writeBenchTiming(
        "load_ms",
        chrono::duration_cast<
            chrono::milliseconds>(
            end - start)
            .count(),
        database.size());

    cout << "Success loading\n";
}

void replayWal()
{
    auto start =
        chrono::high_resolution_clock::now();

    lock_guard<mutex> lock(dbMutex);

    ifstream file(
        WAL_PATH);

    string line;

    while (getline(file, line))
    {
        vector<string> parsedData =
            parseCmd(line);

        if (parsedData.empty())
            continue;

        string cmd =
            parsedData[0];

        transform(cmd.begin(), cmd.end(), cmd.begin(),
                  [](unsigned char c) -> char
                  {
                      return static_cast<char>(std::toupper(c));
                  });

        if (cmd == "SET")
        {
            database[parsedData[1]] =
                parsedData[2];
            insertKey(parsedData[1]);
        }
        else if (cmd == "DEL")
        {
            database.erase(
                parsedData[1]);
            removeKey(parsedData[1]);
        }
        else if (cmd == "EXPIRE")
        {
            auto timestamp =
                stoll(parsedData[2]);

            auto expiryTime =
                chrono::system_clock::time_point(
                    chrono::seconds(
                        timestamp));

            if (
                chrono::system_clock::now() > expiryTime)
            {
                database.erase(
                    parsedData[1]);
                removeKey(parsedData[1]);
                continue;
            }

            expiryMap[parsedData[1]] = expiryTime;
        }
    }

    auto end =
        chrono::high_resolution_clock::now();

    writeBenchTiming(
        "replay_ms",
        chrono::duration_cast<
            chrono::milliseconds>(
            end - start)
            .count());

    cout << "success replay\n";
}