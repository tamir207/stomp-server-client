#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

class Event {
private:
    std::string username;
    // name of team a
    std::string team_a_name;
    // name of team b
    std::string team_b_name;
    // name of the event
    std::string name;
    // time of the event in seconds
    int time;
    // map of all the general game updates
    std::map<std::string, std::string> game_updates;
    // map of all team a updates the second type can be a string bool or int
    std::map<std::string, std::string> team_a_updates;
    // map of all team b updates
    std::map<std::string, std::string> team_b_updates;
    // description of the event
    std::string description;
    // is event after half time
    bool beforeHalftime;

public:
    Event(
        std::string name, std::string team_a_name, std::string team_b_name, int time,
        std::map<std::string, std::string> game_updates, std::map<std::string, std::string> team_a_updates,
        std::map<std::string, std::string> team_b_updates, std::string discription
    );
    Event(const std::string& frame_body);
    virtual ~Event();
    const std::string& get_username() const;
    const std::string& get_team_a_name() const;
    const std::string& get_team_b_name() const;
    const std::string& get_name() const;
    int get_time() const;
    const std::map<std::string, std::string>& get_game_updates() const;
    const std::map<std::string, std::string>& get_team_a_updates() const;
    const std::map<std::string, std::string>& get_team_b_updates() const;
    const std::string& get_description() const;
    bool getBeforeHalftime() const; // TODO
    void make_second_half_time(); // TODO
    void set_username(const std::string& username);
    // Return -1 if this < other, 0 is this == other, 1 if this > other
    bool compareTo(const Event& other) const;
    std::string toString() const;
};

// an object that holds the names of the teams and a vector of events, to be returned by the parseEventsFile function
struct names_and_events {
    std::string team_a_name;
    std::string team_b_name;
    std::vector<Event> events;
};

// function that parses the json file and returns a names_and_events object
names_and_events parseEventsFile(std::string json_path);
