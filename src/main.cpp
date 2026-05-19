#include "persistence.h"
#include "server.h"

int main()
{
    loadDatabase();
    startServer();

    return 0;
}