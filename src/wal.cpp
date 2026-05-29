#include "wal.h"
#include "global.h"
#include <fstream>

using namespace std;

void appendToWal(const string &command)
{
    ofstream file(
        WAL_PATH,
        ios::app);

    if (!file.is_open())
        return;

    file << command << '\n';
}

void clearWal()
{
    ofstream file(WAL_PATH, ios::trunc);
}
