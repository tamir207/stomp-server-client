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
Frame();
Frame(std::string msg);
void addHeader(std::string key, std::string value);
void setType(std::string newType);
void setBody(std::string newBody);
std::string getHeaderValue(std::string key);
std::string getType();
std::string getBody();
std::string toString();
};