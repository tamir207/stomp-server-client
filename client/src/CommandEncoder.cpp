#include "../include/CommandEncoder.h"
#include <string>
#include <vector>

CommandEncoder::CommandEncoder() {}

std::string CommandEncoder::encode(std::string msg)
{
    std::string frame = "";
    std::vector<std::string> words = split(msg, ' ');
    size_t len = words.size();
    
    if (len >= 4)
    {
        if (words[0] == "login")
        {
            size_t colonPos = words[1].find(':');
            if (colonPos != std::string::npos)
            {
                std::string host = words[1].substr(0, colonPos);
                std::string port = words[1].substr(colonPos + 1);
                std::string username = words[2];
                std::string passcode = words[3];

                frame = "CONNECT\n";
                frame += "accept-version:1.2\n";
                frame += "host:" + host + "\n";
                frame += "login:" + username + "\n";
                frame += "passcode:" + passcode + "\n";
                frame += "\n\n";
                frame += "\0";
            }
        }
    }
    return frame;
}

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