#include "../include/Utils.h"

namespace Utils {
std::vector<std::string> splitNoEmpty(const std::string& s, char delimiter) {
    std::vector<std::string> words;
    std::string currentWord = "";

    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == delimiter) {
            if (currentWord != "") {
                words.push_back(currentWord);
                currentWord = "";
            }
        } else {
            currentWord += s[i];
        }
    }

    if (currentWord != "") {
        words.push_back(currentWord);
    }

    return words;
}

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string& s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string& s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string& s) { return rtrim(ltrim(s)); }

bool writeStringToFile(const std::string& fileName, const std::string& content) {
    std::ofstream file(fileName, std::ios::out | std::ios::trunc);

    if (!file.is_open()) {
        return false;
    }

    file << content;
    file.close();

    return true;
}
}
