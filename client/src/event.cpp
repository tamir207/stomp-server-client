#include "../include/event.h"
#include "../include/json.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
using json = nlohmann::json;

Event::Event(
    std::string team_a_name, std::string team_b_name, std::string name, int time,
    std::map<std::string, std::string> game_updates, std::map<std::string, std::string> team_a_updates,
    std::map<std::string, std::string> team_b_updates, std::string discription
)
    : username(),
      team_a_name(team_a_name),
      team_b_name(team_b_name),
      name(name),
      time(time),
      game_updates(game_updates),
      team_a_updates(team_a_updates),
      team_b_updates(team_b_updates),
      description(discription),
      beforeHalftime(true) { }

Event::~Event() { }

const std::string& Event::get_username() const { return this->username; }

const std::string& Event::get_team_a_name() const { return this->team_a_name; }

const std::string& Event::get_team_b_name() const { return this->team_b_name; }

const std::string& Event::get_name() const { return this->name; }

int Event::get_time() const { return this->time; }

const std::map<std::string, std::string>& Event::get_game_updates() const { return this->game_updates; }

const std::map<std::string, std::string>& Event::get_team_a_updates() const { return this->team_a_updates; }

const std::map<std::string, std::string>& Event::get_team_b_updates() const { return this->team_b_updates; }

const std::string& Event::get_description() const { return this->description; }

bool Event::getBeforeHalftime() const { return this->beforeHalftime; }

bool Event::compareTo(const Event& other) const {
    if (beforeHalftime != other.getBeforeHalftime()) {
        return beforeHalftime; 
    }
    return time < other.get_time();
}

Event::Event(const std::string& frame_body)
    : team_a_name(""),
      team_b_name(""),
      name(""),
      time(0),
      game_updates(),
      team_a_updates(),
      team_b_updates(),
      description("") { }

names_and_events parseEventsFile(std::string json_path) {
    std::ifstream f(json_path);
    json data = json::parse(f);

    std::string team_a_name = data["team a"];
    std::string team_b_name = data["team b"];

    // run over all the events and convert them to Event objects
    std::vector<Event> events;
    for (auto& event : data["events"]) {
        std::string name = event["event name"];
        int time = event["time"];
        std::string description = event["description"];
        std::map<std::string, std::string> game_updates;
        std::map<std::string, std::string> team_a_updates;
        std::map<std::string, std::string> team_b_updates;
        for (auto& update : event["general game updates"].items()) {
            if (update.value().is_string())
                game_updates[update.key()] = update.value();
            else
                game_updates[update.key()] = update.value().dump();
        }

        for (auto& update : event["team a updates"].items()) {
            if (update.value().is_string())
                team_a_updates[update.key()] = update.value();
            else
                team_a_updates[update.key()] = update.value().dump();
        }

        for (auto& update : event["team b updates"].items()) {
            if (update.value().is_string())
                team_b_updates[update.key()] = update.value();
            else
                team_b_updates[update.key()] = update.value().dump();
        }

        events.push_back(
            Event(team_a_name, team_b_name, name, time, game_updates, team_a_updates, team_b_updates, description)
        );
    }
    names_and_events events_and_names { team_a_name, team_b_name, events };

    return events_and_names;
}

std::string Event::toString() const {
    std::string eventString = "";
    eventString += "username: " + this->get_username() + "\n";
    eventString += "team a: " + this->get_team_a_name() + "\n";
    eventString += "team b: " + this->get_team_b_name() + "\n";
    eventString += "event name: " + this->get_name() + "\n";
    eventString += "time: " + std::to_string(this->get_time()) + "\n";
    eventString += "general game updates:\n";
    for (const auto& update : this->get_game_updates()) {
        eventString += "    " + update.first + ": " + update.second + "\n";
    }
    eventString += "team a updates:\n";
    for (const auto& update : this->get_team_a_updates()) {
        eventString += "    " + update.first + ": " + update.second + "\n";
    }
    eventString += "team b updates:\n";
    for (const auto& update : this->get_team_b_updates()) {
        eventString += "    " + update.first + ": " + update.second + "\n";
    }
    eventString += "description: " + this->get_description();
    return eventString;
}

void Event::set_username(const std::string& username) { this->username = username; }
void Event::make_second_half_time() { this->beforeHalftime = false; }