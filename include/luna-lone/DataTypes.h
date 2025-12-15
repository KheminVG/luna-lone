#pragma once
#include <string>

struct AudioEntry {
    int id;
    std::string displayName;
    std::string filePath;
};

struct TagEntry {
    int id;
    std::string tagName;
};