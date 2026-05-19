#include "parser.h"
#include <sstream>
using namespace std;

vector<string> parseCmd(const string &input)
{
    vector<string> parsedData;

    stringstream ss(input);
    string token;

    ss >> token; // command
    parsedData.push_back(token);

    if (!(ss >> token)) // key may or may not exist
        return parsedData;
    parsedData.push_back(token);

    token.clear();

    getline(ss, token);

    if (!token.empty() && token[0] == ' ')
        token.erase(0, 1);

    if (!token.empty())
        parsedData.push_back(token);

    return parsedData;
}