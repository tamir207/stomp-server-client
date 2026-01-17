#include "../include/CommandEncoder.h"
#include "../include/Utils.h"
#include "../include/Frame.h"
#include <string>
#include <vector>

CommandEncoder::CommandEncoder() {}

std::string CommandEncoder::encode(std::string msg)
{
    Frame frame;
    std::vector<std::string> words = Utils::split(msg, ' ');
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
                
                frame.setType("CONNECT");
                frame.addHeader("accept-version", "1.2");
                frame.addHeader("host", host);
                frame.addHeader("login", username);
                frame.addHeader("passcode", passcode);
            }
        }
    }
    return frame.toString();
}