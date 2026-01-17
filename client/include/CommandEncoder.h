#pragma once
#include <string>
#include <iostream>
#include <vector>

class CommandEncoder
{
private:
public:
    CommandEncoder();
    void encode(std::string msg);
    std::vector<std::string> split(std::string s, char delimiter);
};