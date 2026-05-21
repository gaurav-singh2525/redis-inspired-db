#include "db.h"
#include "client_handler.h"
#include "logger.h"

#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


using namespace std;

void handleClient(int client_fd, int clientID)
{

    string pending;

    while (true)
    {
        char buffer[1024];

        int bytes =
            recv(client_fd,
                 buffer,
                 sizeof(buffer) - 1,
                 0);

        if (bytes <= 0)
        {
            logMessage(
                "Client:" + to_string(clientID) + " disconnected");

            break;
        }

        buffer[bytes] = '\0';

        pending += buffer;

        size_t pos;

        while ((pos = pending.find('\n')) != string::npos)
        {
            string command =
                pending.substr(0, pos);

            pending.erase(0, pos + 1);

            string response =
                cmdDispatcher(
                    command,
                    clientID);

            response += "\n";

            send(client_fd,
                 response.c_str(),
                 response.size(),
                 0);
        }
    }

    close(client_fd);
}