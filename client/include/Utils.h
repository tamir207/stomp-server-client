#pragma once
#include <string>
#include <vector>
#include <fstream>

namespace Utils {
    std::vector<std::string> splitNoEmpty(const std::string& s, char delimiter);
    std::string ltrim(const std::string& s);
    std::string rtrim(const std::string& s);
    std::string trim(const std::string& s);
    bool writeStringToFile(const std::string& fileName, const std::string& content);
}