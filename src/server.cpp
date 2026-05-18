#include "server.h"
#include "client_handler.h"
#include <iostream>
#include <thread>
#include <atomic>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

atomic<int> clientCount = 0;

void startServer()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("socket");
        return;
    }

    sockaddr_in serverAddr{};

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // forces server to start
    int opt = 1;

    setsockopt(server_fd,
               SOL_SOCKET,
               SO_REUSEADDR,
               &opt,
               sizeof(opt));

    if (bind(server_fd,
             (sockaddr *)&serverAddr,
             sizeof(serverAddr)) < 0)
    {
        perror("bind");
        return;
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        return;
    }

    cout << "Server listening on port 9000...\n";

    while (true)
    {
        int client_fd = accept(server_fd, NULL, NULL);

        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }
        cout << "new client connected" << endl;

        int clientID = ++clientCount;

        thread t(handleClient, client_fd, clientID);

        t.detach();
    }

    close(server_fd);
}