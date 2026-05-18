#include "db.h"
#include "client_handler.h"
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>

using namespace std;

void handleClient(int client_fd, int clientID)
{

    cout << "ClientID:" << clientID << " has thread id:" << this_thread::get_id() << endl;

    while (true)
    {
        char buffer[1024];

        int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes <= 0)
        {
            cout << "Client:" << clientID << " Disconnected.\n";

            break;
        }

        buffer[bytes] = '\0';

        string rtnBuffer = cmdDispatcher(buffer, clientID);
        rtnBuffer += "\n";

        send(client_fd,
             rtnBuffer.c_str(),
             rtnBuffer.size(),
             0);
    }

    close(client_fd);
}