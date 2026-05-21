#pragma once

#include <string>

void touchKey(
    const std::string &key);

void insertKey(
    const std::string &key);

void removeKey(
    const std::string &key);

void evictIfNeeded();

std::string formatLruList();