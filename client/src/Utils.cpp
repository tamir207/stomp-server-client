#include "../include/Utils.h"
#include <sstream>

std::vector<std::string> CommandEncoder::split(std::string s, char delimiter)
{
    std::vector<std::string> words;
    std::string currentWord = "";

    for (size_t i = 0; i < s.length(); i++)
    {
        if (s[i] == delimiter)
        {
            if (currentWord != "")
            {
                words.push_back(currentWord);
                currentWord = "";
            }
        }
        else
        {
            currentWord += s[i];
        }
    }

    if (currentWord != "")
    {
        words.push_back(currentWord);
    }

    return words;
}