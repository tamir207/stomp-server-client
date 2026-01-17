#pragma once
#include <string>
#include <iostream>
#include <vector>

class CommandEncoder
{
private:
public:
    CommandEncoder();
    std::string encode(std::string msg);
};