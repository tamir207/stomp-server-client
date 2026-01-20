#include "../include/Game.h"
#include "../include/Utils.h"
#include <algorithm>
#include <ostream>

Game::Game(const std::string& name)
    : gameName(name),
      events(),
      team_a_name(""),
      team_b_name("") {

    size_t pos = name.find('_');
    if (pos != std::string::npos) {
        team_a_name = name.substr(0, pos);
        team_b_name = name.substr(pos + 1);
    }
}

const std::string& Game::get_team_a_name() const { return this->team_a_name; }
const std::string& Game::get_team_b_name() const { return this->team_b_name; }

std::string Game::summarize(const std::string& username) {

    std::vector<Event> userEvents;
    for (const auto& currentEvent : events) {
        if (currentEvent.get_username() == username) {
            userEvents.push_back(currentEvent);
        }
    }

    if (userEvents.empty()) {
        return "";
    }

    std::string res = team_a_name + " vs " + team_b_name + "\n";
    res += "Game stats:\n";

    std::map<std::string, std::string> gameUpdates;
    std::map<std::string, std::string> teamAUpdates;
    std::map<std::string, std::string> teamBUpdates;

    for (const auto& currentEvent : userEvents) {
        for (const auto& update : currentEvent.get_game_updates()) {
            gameUpdates[update.first] = update.second;
        }
        for (const auto& update : currentEvent.get_team_a_updates()) {
            teamAUpdates[update.first] = update.second;
        }
        for (const auto& update : currentEvent.get_team_b_updates()) {
            teamBUpdates[update.first] = update.second;
        }
    }

    res += "General stats:\n";
    for (const auto& update : gameUpdates) {
        res += update.first + ": " + update.second + "\n";
    }

    res += team_a_name + " stats:\n";
    for (const auto& update : teamAUpdates) {
        res += update.first + ": " + update.second + "\n";
    }

    res += team_b_name + " stats:\n";
    for (const auto& update : teamBUpdates) {
        res += update.first + ": " + update.second + "\n";
    }

    res += "Game event reports:\n";
    for (const auto& currentEvent : userEvents) {
        res += std::to_string(currentEvent.get_time()) + " - " + currentEvent.get_name() + ":\n";
        res += currentEvent.get_description() + "\n\n";
    }

    return res;
}

std::string Game::print_events() const {
    std::string res;
    res += "--------------------------------------------\n";
    for (const auto& event : events) {
        res += event.toString();
        res += "\n\n";
    }
    res += "--------------------------------------------\n";
    return res;
}

std::string Game::generateReport(const std::string& username, names_and_events parsed) {
    std::string report;
    for (const auto& event : parsed.events) {
        if (username == event.get_username()) {
            report += event.toString();
            report += "\n\n";
        }
    }

    return report;
}

void Game::addEvents(const std::string& frameBody) {
    std::vector<std::string> lines = Utils::splitWithEmpty(frameBody, '\n');
    std::vector<Event> newEvents;
    std::string username;
    std::string team_a_name;
    std::string team_b_name;
    std::string name;
    int time;
    std::map<std::string, std::string> game_updates;
    std::map<std::string, std::string> team_a_updates;
    std::map<std::string, std::string> team_b_updates;
    std::string description;
    bool beforeHalftime;
    size_t i = 0;

    // for (size_t j = 0; j < lines.size(); j++) {
    //     std::cout << "-------------------" << std::endl;
    //     std::cout << lines[j] << std::endl;
    // }

    for (; i < lines.size();) {
        if (!lines[i].empty()) {
            std::vector<std::string> splittedLine = Utils::splitNoEmpty(lines[i], ':');
            std::string key = splittedLine[0];
            if (key == "username") {
                username = splittedLine[1];
            } else if (key == "team a") {
                team_a_name = splittedLine[1];
            } else if (key == "team b") {
                team_b_name = splittedLine[1];
            } else if (key == "event name") {
                name = splittedLine[1];
            } else if (key == "time") { // key == "time"
                time = std::stoi(splittedLine[1]);
            } else if (key == "general game updates") {
                i++;
                for (; lines[i][0] == ' ' && lines[i][1] == ' ' && lines[i][2] == ' ' && lines[i][3] == ' '; i++) {
                    std::vector<std::string> splittedUpdateLines = Utils::splitNoEmpty(lines[i], ':');
                    game_updates[splittedUpdateLines[0]] = splittedUpdateLines[1];
                }
            } else if (key == "team a updates") {
                i++;
                for (; lines[i][0] == ' ' && lines[i][1] == ' ' && lines[i][2] == ' ' && lines[i][3] == ' '; i++) {
                    std::vector<std::string> splittedUpdateLines = Utils::splitNoEmpty(lines[i], ':');
                    team_a_updates[splittedUpdateLines[0]] = splittedUpdateLines[1];
                }
            } else if (key == "team b updates") {
                i++;
                for (; lines[i][0] == ' ' && lines[i][1] == ' ' && lines[i][2] == ' ' && lines[i][3] == ' '; i++) {
                    std::vector<std::string> splittedUpdateLines = Utils::splitNoEmpty(lines[i], ':');
                    team_b_updates[splittedUpdateLines[0]] = splittedUpdateLines[1];
                }
            } else if (key == "description") {
                description = splittedLine[1];
            }
        } else {
            if (i == 0 || lines[i - 1].empty())
                continue;

            Event newEvent(
                team_a_name, team_b_name, name, time, game_updates, team_a_updates, team_b_updates, description
            );
            newEvent.set_username(username);

            if (game_updates["    before halftime"] == " false") { // TODO: MUST ADD TRIM()
                newEvent.make_second_half_time();
            }

            // auto it = std::lower_bound(events.begin(), events.end(), newEvent, [](const Event& a, const Event& b) {
            //     return a.compareTo(b) < 0;
            // });
            events.push_back(newEvent);

            username = "";
            team_a_name = "";
            team_b_name = "";
            name = "";
            description = "";
            time = 0;
            game_updates.clear();
            team_a_updates.clear();
            team_b_updates.clear();
        }
        i++;
    }

    std::sort(events.begin(), events.end(), [](const Event& a, const Event& b) { return a.compareTo(b); });
}