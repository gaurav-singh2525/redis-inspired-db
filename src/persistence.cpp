#include "persistence.h"
#include "global.h"
#include "parser.h"

#include <fstream>
#include <iostream>
#include <mutex>
#include <cctype>
#include <algorithm>

using namespace std;

void saveDatabase()
{
    lock_guard<mutex> lock(dbMutex);

    ofstream fout("data/store.cdb");

    if (!fout.is_open())
    {
        cerr << "Failed to open file\n";
        return;
    }

    for (const auto &[key, value] : database)
    {
        fout << key << "=" << value << "\n";
    }
    cout << "saved success\n";
}

void loadDatabase()
{
    lock_guard<mutex> lock(dbMutex);

    ifstream fin("data/store.cdb");

    if (!fin.is_open())
    {
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
    }
    cout << "Success loading\n";
}

void replayWal()
{
    lock_guard<mutex> lock(dbMutex);

    ifstream file(
        "data/wal.log");

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
        }
        else if (cmd == "DEL")
        {
            database.erase(
                parsedData[1]);
        }
        else if (cmd == "CLEAR")
        {
            database.clear();
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

                expiryMap.erase(
                    parsedData[1]);

                continue;
            }

            expiryMap[parsedData[1]] = expiryTime;
        }
    }
    cout << "success replay\n";
}