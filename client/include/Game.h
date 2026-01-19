#pragma once

#include "../include/event.h"

#include <string>
#include <iostream>

class Game
{
private:
std::vector<std::map<std::string, Event>> events; // vector<username, event>
std::string gameName;
// TODO: add game stats.

public:
void summrize();
Event getEventOfUser();
};