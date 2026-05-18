#include "db.h"
#include "parser.h"
#include <unordered_map>
#include <cctype>
#include <algorithm>
#include <mutex>
#include <iostream>

mutex dbMutex;
mutex coutMutex;

unordered_map<string, string> database;

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
            database[parsedData[1]] = parsedData[2];
        }

        {
            lock_guard<mutex> lock(coutMutex);
            cout << "Client:" << clientID
                 << " set " << parsedData[1]
                 << " = " << parsedData[2] << endl;
        }

        return "OK";
    }
    else if (cmd == "GET")
    {

        if (parsedData.size() != 2)
            return "INVALID COMMAND";

        string value;

        {
            lock_guard<mutex> lock(dbMutex);

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

            database.erase(parsedData[1]);
        }

        {
            lock_guard<mutex> lock(coutMutex);
            cout << "Client:" << clientID
                 << " deleted " << parsedData[1] << endl;
        }

        return "OK";
    }
    else if (cmd == "COUNT")
    {

        if (parsedData.size() != 1)
            return "INVALID COMMAND";

        {
            lock_guard<mutex> lock(dbMutex);
            return to_string(database.size());
        }
    }
    else if (cmd == "CLEAR")
    {
        {
            lock_guard<mutex> lock(dbMutex);

            database.clear();
        }
        return "OK";
    }

    return "INVALID COMMAND";
}