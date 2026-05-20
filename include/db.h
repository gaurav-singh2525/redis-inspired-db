#pragma once
#include <string>

std::string cmdDispatcher(const std::string &input, int clientID);
bool isExpired(const std::string &key);