#include "config.h"
#include "global.h"
#include "logger.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void loadConfig()
{
    ifstream fin("configs/server.conf");

    if (!fin.is_open())
    {
        logMessage("Config file not found. Using defaults");
        MAX_CAPACITY =
            static_cast<size_t>(
                CACHE_CAPACITY);
        return;
    }

    string line;

    while (getline(fin, line))
    {
        if (line.empty())
            continue;

        size_t pos =
            line.find('=');

        if (pos == string::npos)
            continue;

        string key =
            line.substr(0, pos);

        string value =
            line.substr(pos + 1);

        try
        {
            if (key == "port")
            {
                PORT =
                    stoi(value);
            }
            else if (key ==
                     "cache_capacity")
            {
                CACHE_CAPACITY =
                    stoi(value);
            }
            else if (key ==
                     "wal_path")
            {
                WAL_PATH =
                    value;
            }
            else if (key ==
                     "db_path")
            {
                DB_PATH =
                    value;
            }
        }
        catch (...)
        {
            logMessage("Invalid config: " + line);
        }
    }

    logMessage("Config loaded");
    MAX_CAPACITY =
        static_cast<size_t>(
            CACHE_CAPACITY);
}