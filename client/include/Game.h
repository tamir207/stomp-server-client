#pragma once

#include "../include/event.h"

#include <iostream>
#include <string>

class Game {
private:
    // Invariant: events are always ordered.
    std::vector<Event> events;
    std::string gameName;
    std::string team_a_name;
    std::string team_b_name;

public:
    Game();
    Game(const std::string& gameName);
    // Returns a formmated string summary of the events sent by a speific user.
    std::string summarize(const std::string& username);
    // When receiving MESSAGE, pass the body here. This will add it to events with the corresponding user name.
    void addEvent(const std::string& singleEvent);
    // Retreive the event of a specific user
    const std::string& get_team_a_name() const;
    const std::string& get_team_b_name() const;
    std::string print_events() const;
};