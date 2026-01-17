#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>

class Frame{
private:
std::string type;
std::map<std::string, std::string> headers;
std::string body;

public:
Frame(std::string msg);
std::string getHeaderValue(std::string key);
std::string getType();
std::string getBody();
};