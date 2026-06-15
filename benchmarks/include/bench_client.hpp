#pragma once

#include <string>

namespace bench
{

class Client
{
public:
    Client(const std::string &host, int port);
    ~Client();

    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;

    void connect();
    void close();
    bool isConnected() const;

    std::string command(const std::string &cmd);
    std::string commandTimed(
        const std::string &cmd,
        double &latencyMicros);

private:
    std::string readLine();
    void writeAll(const std::string &data);

    std::string host_;
    int port_;
    int fd_ = -1;
};

void populateKeys(
    Client &client,
    std::size_t count,
    const std::string &prefix = "benchkey");

bool expectResponse(
    const std::string &actual,
    const std::string &expected);

std::size_t queryCapacity(Client &client);

std::size_t effectiveKeyCount(
    Client &client,
    std::size_t desired);

} // namespace bench
