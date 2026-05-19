#include "persistence.h"
#include "global.h"
#include <fstream>
#include <iostream>
#include <mutex>
#include <unordered_map>

using namespace std;

void saveDatabase()
{
    lock_guard<mutex> lock(dbMutex);

    ofstream fout;
    fout.open("store.cdb");

    if (!fout.is_open())
    {
        cerr << "Failed to open store.cdb\n";
        return;
    }

    for (const auto &[key, value] : database)
    {
        fout << key << "=" << value << "\n";
    }
    fout.close();
}

void loadDatabase()
{
    lock_guard<mutex> lock(dbMutex);

    ifstream fin;
    fin.open("store.cdb");

    if (!fin.is_open())
    {
        return;
    }

    database.clear();
    
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
    fin.close();
}