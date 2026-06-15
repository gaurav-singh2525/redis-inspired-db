#pragma once

#include <string>

namespace bench
{

struct Config
{
    std::string host = "127.0.0.1";
    int port = 9000;
    std::string projectRoot = "..";
    std::string serverBinary = "../db_server";
    std::string timingFile = "../data/bench_timing.txt";
};

Config loadConfig();

} // namespace bench
