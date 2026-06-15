#include "bench_config.hpp"

#include <fstream>
#include <sstream>

namespace bench
{

Config loadConfig()
{
    Config cfg;

    std::ifstream fin("../configs/server.conf");

    if (!fin.is_open())
        return cfg;

    std::string line;

    while (std::getline(fin, line))
    {
        const auto pos = line.find('=');

        if (pos == std::string::npos)
            continue;

        const std::string key =
            line.substr(0, pos);

        const std::string value =
            line.substr(pos + 1);

        if (key == "port")
            cfg.port = std::stoi(value);
    }

    return cfg;
}

} // namespace bench
