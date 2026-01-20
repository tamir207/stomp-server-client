#include "../include/Utils.h"
#include <sstream>

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

std::vector<std::string> splitWithEmpty(const std::string& s, char delimiter) {
    std::vector<std::string> words;
    std::string currentWord = "";

    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == delimiter) {
            words.push_back(currentWord);
            currentWord = "";

        } else {
            currentWord += s[i];
        }
    }

    return words;
}
}
