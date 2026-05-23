#include "resp.h"

#include <sstream>

using namespace std;

vector<string>
parseRESP(const string &input)
{
    vector<string> parsedData;

    stringstream ss(input);

    string line;

    getline(ss, line);

    // must begin with *count
    if (line.empty() || line[0] != '*')
        return {};

    int argCount =
        stoi(line.substr(1));

    for (int i = 0; i < argCount; i++)
    {
        // read $len
        getline(ss, line);

        if (line.empty() || line[0] != '$')
            return {};

        // read actual token
        getline(ss, line);

        if (!line.empty() &&
            line.back() == '\r')
        {
            line.pop_back();
        }

        parsedData.push_back(line);
    }

    return parsedData;
}