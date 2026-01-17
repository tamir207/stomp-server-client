#include "../include/CommandEncoder.h"
#include <string>
#include <vector>

CommandEncoder::CommandEncoder() {}

void CommandEncoder::encode(std::string msg)
{
    std::vector<std::string> words = split(msg, ' ');
    for (size_t i = 0; i < words.size(); i++){
        std::cout << "Word " << i << ": " << words[i] << std::endl;
    }
}

std::vector<std::string> CommandEncoder::split(std::string s, char delimiter)
{
    std::vector<std::string> words;
    std::string currentWord = "";

    for (size_t i = 0; i < s.length(); i++){
        if (s[i] == delimiter){
            if (currentWord != ""){
                words.push_back(currentWord);
                currentWord = "";
            }
        }
        else{
            currentWord += s[i];
        }
    }

    if (currentWord != ""){
        words.push_back(currentWord);
    }

    return words;
}