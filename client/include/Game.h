#pragma once

#include "../include/event.h"

#include <string>
#include <iostream>

class Game
{
private:
std::vector<std::map<std::string, Event>> events; // vector<username, event>
std::string gameName;

public:
Game(const std::string& gameName);
// Returns a formmated string summary of the events sent by a speific user.
std::string summrize(const std::string& userName);
// Return a formmated string event ready to send to server
std::string generateReport(const std::string& userName, names_and_events parsed);
// When receiving MESSAGE, pass the body here. This will add it to events with the corresponding user name.
void addEvent(const std::string& frameBody);
// Retreive the event of a specific user
};