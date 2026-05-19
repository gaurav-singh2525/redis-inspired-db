#include "wal.h"

#include <fstream>

using namespace std;

void appendToWal(const string &command)
{
    ofstream file(
        "data/wal.log",
        ios::app);

    if (!file.is_open())
        return;

    file << command << '\n';
}

void clearWal()
{
    ofstream file("data/wal.log", ios::trunc);
}
