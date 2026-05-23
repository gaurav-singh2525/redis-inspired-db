#include "db.h"
#include "client_handler.h"
#include "logger.h"

#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>

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

        while (true)
        {
            if (pending.empty())
                break;

            // RESP request
            if (pending[0] == '*')
            {
                size_t firstLineEnd =
                    pending.find('\n');

                if (firstLineEnd == string::npos)
                    break;

                int argc =
                    stoi(
                        pending.substr(
                            1,
                            firstLineEnd - 1));

                size_t currentPos =
                    firstLineEnd + 1;

                bool complete =
                    true;

                vector<string> args;

                for (int i = 0; i < argc; i++)
                {
                    size_t lenEnd =
                        pending.find(
                            '\n',
                            currentPos);

                    if (lenEnd == string::npos)
                    {
                        complete = false;
                        break;
                    }

                    currentPos =
                        lenEnd + 1;

                    size_t argEnd =
                        pending.find(
                            '\n',
                            currentPos);

                    if (argEnd == string::npos)
                    {
                        complete = false;
                        break;
                    }

                    string arg =
                        pending.substr(
                            currentPos,
                            argEnd - currentPos);

                    if (!arg.empty() &&
                        arg.back() == '\r')
                    {
                        arg.pop_back();
                    }

                    args.push_back(arg);

                    currentPos =
                        argEnd + 1;
                }

                if (!complete)
                    break;

                pending.erase(0, currentPos);

                string command;

                for (size_t i = 0;
                     i < args.size();
                     i++)
                {
                    command += args[i];

                    if (i + 1 < args.size())
                        command += " ";
                }

                string response =
                    cmdDispatcher(
                        command,
                        clientID);

                response += "\n";

                send(
                    client_fd,
                    response.c_str(),
                    response.size(),
                    0);
            }

            // Normal text command
            else
            {
                size_t pos =
                    pending.find('\n');

                if (pos == string::npos)
                    break;

                string command =
                    pending.substr(0, pos);

                pending.erase(0, pos + 1);

                if (!command.empty() &&
                    command.back() == '\r')
                {
                    command.pop_back();
                }

                string response =
                    cmdDispatcher(command, clientID);

                response += "\n";

                send(client_fd,
                     response.c_str(),
                     response.size(),
                     0);
            }
        }
    }

    close(client_fd);
}