#include "bench_client.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cstring>
#include <sstream>
#include <stdexcept>

namespace bench
{

Client::Client(
    const std::string &host,
    int port)
    : host_(host),
      port_(port)
{
}

Client::~Client()
{
    close();
}

void Client::connect()
{
    fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (fd_ < 0)
        throw std::runtime_error(
            std::string("socket: ") +
            strerror(errno));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port_));

    if (inet_pton(AF_INET,
                  host_.c_str(),
                  &addr.sin_addr) <= 0)
        throw std::runtime_error(
            "invalid host address");

    if (::connect(fd_,
                  reinterpret_cast<sockaddr *>(&addr),
                  sizeof(addr)) < 0)
        throw std::runtime_error(
            std::string("connect: ") +
            strerror(errno));
}

void Client::close()
{
    if (fd_ >= 0)
    {
        ::close(fd_);
        fd_ = -1;
    }
}

bool Client::isConnected() const
{
    return fd_ >= 0;
}

void Client::writeAll(const std::string &data)
{
    std::size_t sent = 0;

    while (sent < data.size())
    {
        const ssize_t n =
            ::send(fd_,
                   data.data() + sent,
                   data.size() - sent,
                   0);

        if (n <= 0)
            throw std::runtime_error(
                "send failed");

        sent += static_cast<std::size_t>(n);
    }
}

std::string Client::readLine()
{
    std::string line;

    while (true)
    {
        char ch = '\0';
        const ssize_t n =
            recv(fd_, &ch, 1, 0);

        if (n <= 0)
            throw std::runtime_error(
                "connection closed while reading response");

        line.push_back(ch);

        if (ch == '\n')
            break;
    }

    if (!line.empty() &&
        line.back() == '\n')
    {
        line.pop_back();
    }

    if (!line.empty() &&
        line.back() == '\r')
    {
        line.pop_back();
    }

    return line;
}

std::string Client::command(
    const std::string &cmd)
{
    double ignored = 0.0;
    return commandTimed(cmd, ignored);
}

std::string Client::commandTimed(
    const std::string &cmd,
    double &latencyMicros)
{
    const auto start =
        std::chrono::high_resolution_clock::now();

    writeAll(cmd + "\n");
    const std::string response = readLine();

    const auto end =
        std::chrono::high_resolution_clock::now();

    latencyMicros =
        std::chrono::duration<double, std::micro>(
            end - start)
            .count();

    return response;
}

void populateKeys(
    Client &client,
    std::size_t count,
    const std::string &prefix)
{
    for (std::size_t i = 0; i < count; ++i)
    {
        std::ostringstream cmd;
        cmd << "SET " << prefix << i
            << " value" << i;

        const std::string response =
            client.command(cmd.str());

        if (response != "OK")
        {
            throw std::runtime_error(
                "populate failed at key " +
                std::to_string(i) +
                " response=" + response);
        }
    }
}

bool expectResponse(
    const std::string &actual,
    const std::string &expected)
{
    return actual == expected;
}

std::size_t queryCapacity(Client &client)
{
    return std::stoull(client.command("CAPACITY"));
}

std::size_t effectiveKeyCount(
    Client &client,
    std::size_t desired)
{
    const std::size_t capacity =
        queryCapacity(client);

    return desired < capacity
               ? desired
               : capacity;
}

} // namespace bench
