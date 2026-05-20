#include "db.h"
#include "parser.h"
#include "global.h"
#include "wal.h"
#include "logger.h"

#include <cctype>
#include <algorithm>
#include <iostream>

using namespace std;

string cmdDispatcher(const string &input, int clientID)
{
    const auto parsedData = parseCmd(input);

    if (parsedData.empty() || parsedData[0].empty())
    {
        return "INVALID COMMAND";
    }

    string cmd = parsedData[0];

    transform(cmd.begin(), cmd.end(), cmd.begin(),
              [](unsigned char c) -> char
              {
                  return static_cast<char>(std::toupper(c));
              });

    if (cmd == "PING")
    {
        if (parsedData.size() != 1)
            return "INVALID COMMAND";

        return "PONG";
    }
    else if (cmd == "SET")
    {

        if (parsedData.size() != 3)
            return "INVALID COMMAND";

        {
            lock_guard<mutex> lock(dbMutex);

            appendToWal(input);

            if (expiryMap.find(parsedData[1]) != expiryMap.end())
                expiryMap.erase(parsedData[1]);

            database[parsedData[1]] = parsedData[2];
        }

        logMessage("Client:" + to_string(clientID) + " set key: " + parsedData[1] + " as " + parsedData[2]);

        return "OK";
    }
    else if (cmd == "EXPIRE")
    {
        if (parsedData.size() != 3)
            return "INVALID COMMAND";

        {
            lock_guard<mutex>
                lock(dbMutex);

            if (
                database.find(
                    parsedData[1]) == database.end())
            {
                return "KEY DNE";
            }

            auto time =
                stoi(parsedData[2]);

            auto expiryTime =
                chrono::system_clock::now() + chrono::seconds(time);

            expiryMap[parsedData[1]] = expiryTime;

            long long
                expiryTimestamp =
                    chrono::duration_cast<
                        chrono::seconds>(
                        expiryTime
                            .time_since_epoch())
                        .count();

            string walCmd =
                "EXPIRE " + parsedData[1] + " " + to_string(expiryTimestamp);

            appendToWal(
                walCmd);
        }

        logMessage(
            "Client:" + to_string(clientID) + " set TTL for key: " + parsedData[1] + " to " + parsedData[2] + " seconds");

        return "OK";
    }
    else if (cmd == "TTL")
    {
        if (parsedData.size() != 2)
            return "INVALID COMMAND";

        {
            lock_guard<mutex>
                lock(dbMutex);

            if (isExpired(parsedData[1]))
            {
                return "TTL EXPIRED OR KEY DNE";
            }

            auto it = expiryMap.find(parsedData[1]);

            if (it == expiryMap.end())
            {
                return "NO TTL";
            }
            auto now =
                chrono::system_clock::now();

            auto remaining =
                chrono::duration_cast<
                    chrono::seconds>(
                    it->second - now)
                    .count();

            return to_string(
                remaining);
        }
    }
    else if (cmd == "GET")
    {

        if (parsedData.size() != 2)
            return "INVALID COMMAND";

        string value;

        {
            lock_guard<mutex> lock(dbMutex);

            if (isExpired(parsedData[1]))
            {
                return "NULL";
            }

            auto it = database.find(parsedData[1]);

            if (it == database.end())
                return "NULL";

            value = it->second;
        }

        return value;
    }
    else if (cmd == "DEL")
    {

        if (parsedData.size() != 2)
            return "INVALID COMMAND";

        {
            lock_guard<mutex> lock(dbMutex);

            if (database.find(parsedData[1]) == database.end())
                return "DNE";

            appendToWal(input);

            database.erase(parsedData[1]);
            expiryMap.erase(parsedData[1]);
        }

        logMessage("Client:" + to_string(clientID) + " deleted " + parsedData[1]);

        return "OK";
    }
    else if (cmd == "COUNT")
    {

        if (parsedData.size() != 1)
            return "INVALID COMMAND";

        {
            lock_guard<mutex> lock(dbMutex);
            for (auto it =
                     expiryMap.begin();
                 it != expiryMap.end();)
            {
                if (chrono::system_clock::now() > it->second)
                {
                    database.erase(
                        it->first);

                    it =
                        expiryMap.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            return to_string(database.size());
        }
    }
    else if (cmd == "CLEAR")
    {
        {
            lock_guard<mutex> lock(dbMutex);
            appendToWal(input);
            clearWal();
            database.clear();
            expiryMap.clear();
        }
        return "OK";
    }

    return "INVALID COMMAND";
}

bool isExpired(
    const string &key)
{
    auto it =
        expiryMap.find(key);

    if (it == expiryMap.end())
        return false;

    if (chrono::system_clock::now() > it->second)
    {
        database.erase(key);

        expiryMap.erase(key);

        return true;
    }

    return false;
}
