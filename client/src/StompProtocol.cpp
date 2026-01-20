#include "../include/StompProtocol.h"
#include "../include/Frame.h"
#include "../include/NetworkClient.h"
#include "../include/Utils.h"
#include "../include/event.h"
#include "../include/json.hpp"
#include <fstream>

StompProtocol::StompProtocol()
    : subIDCounter(1),
      receiptCounter(1),
      channelToId(),
      idToChannel(),
      connected(false),
      receiptToStomp(),
      games(),
      networkClient(),
      gameUpdates(),
      report(false),
      username() { }

StompProtocol::~StompProtocol() { networkClient.disconnect(); }

bool StompProtocol::handleServerInput(std::string msg) {
    Frame frame = Frame(msg);
    std::cout << "------------STOMP-RECEIVED------------" << std::endl;
    std::cout << msg << std::endl;
    std::cout << "------------STOMP-RECEIVED------------" << std::endl;
    if (frame.getType() == "ERROR") {
        std::cout << frame.getHeaderValue("message") << std::endl;
        return true;
    } else if (frame.getType() == "CONNECTED") {
        std::cout << "Login successful" << std::endl;
        connected = true;
    } else if (frame.getType() == "MESSAGE") {
        auto it = games.find(frame.getHeaderValue("destination"));
        if (it != games.end()) {
            Game& game = it->second;
            game.addEvents(frame.getBody());
        }
    } else if (frame.getType() == "RECEIPT") {
        Frame receiptAttachedFrame = receiptToStomp[frame.getHeaderValue("receipt-id")];
        std::cout << "Receipt Stomp Command: " << receiptAttachedFrame.getType() << std::endl;
        if (receiptAttachedFrame.getType() == "DISCONNECT") {
            std::cout << "Disconnected" << std::endl;
            channelToId.clear();
            idToChannel.clear();
            username = "";
            connected = false;
            return true;
        } else if (receiptAttachedFrame.getType() == "SUBSCRIBE") {
            std::cout << "Joined channel " << receiptAttachedFrame.getHeaderValue("destination") << std::endl;
        }
    }

    return false;
}

void StompProtocol::handleUserInput(std::string command) {
    Frame frame;
    std::vector<std::string> words = Utils::splitNoEmpty(command, ' ');
    size_t len = words.size();

    if (command.size() == 0) {
        return;
    }

    bool isValidCommand = false;
    if (len >= 4) {
        if (words[0] == "login") {
            if (connected) {
                std::cout << "The client is already logged in, log out before trying again\n" << std::endl;
                return;
            }
            size_t colonPos = words[1].find(':');
            if (colonPos != std::string::npos) {
                std::string host = words[1].substr(0, colonPos);
                std::string port = words[1].substr(colonPos + 1);
                std::string username = words[2];
                std::string passcode = words[3];
                short shortPort = static_cast<short>(std::stoi(port));
                networkClient.connect(host, shortPort, [this](std::string msg) {
                    return this->handleServerInput(msg);
                });

                frame.setType("CONNECT");
                frame.addHeader("accept-version", "1.2");
                frame.addHeader("host", host);
                frame.addHeader("login", username);
                frame.addHeader("passcode", passcode);
                frame.addHeader("receipt", std::to_string(receiptCounter));
                receiptToStomp[std::to_string(receiptCounter)] = frame;
                isValidCommand = true;
                receiptCounter++;
                this->username = username;
            }
        } else if (words[0] == "summary") {
            std::string gameName = words[1];
            std::string userName = words[2];
            std::string filePath = words[3];

            if (games.find(gameName) != games.end()) {
                std::string summaryOutput = games.at(gameName).summarize(userName);

                if (summaryOutput.empty()) {
                    std::cout << "The user " << userName << " has not reported on the game " << gameName << std::endl;
                } else {
                    std::ofstream fileStream(filePath);
                    if (fileStream.is_open()) {
                        fileStream << summaryOutput;
                        fileStream.close();
                    }
                }
            }
        }
    } else if (len >= 2) {
        if (words[0] == "join") {
            if (!connected) {
                std::cout << "Please login first" << std::endl;
                return;
            }

            std::string channel = words[1];
            if (channelToId.find(channel) == channelToId.end() && connected) {
                frame.setType("SUBSCRIBE");
                frame.addHeader("destination", channel);
                frame.addHeader("id", std::to_string(subIDCounter));
                frame.addHeader("receipt", std::to_string(receiptCounter));
                channelToId[channel] = subIDCounter;
                idToChannel[subIDCounter] = channel;
                receiptToStomp[std::to_string(receiptCounter)] = frame;
                subIDCounter++;
                receiptCounter++;
                isValidCommand = true;
            } else {
                std::cout << "You are already subscribed to " << channel << std::endl;
            }
        } else if (words[0] == "exit") {
            if (!connected) {
                std::cout << "Please login first" << std::endl;
                return;
            }

            std::string channel = words[1];
            auto ind = channelToId.find(channel);
            if (ind != channelToId.end()) {
                int id = ind->second;
                frame.setType("UNSUBSCRIBE");
                frame.addHeader("id", std::to_string(id));
                frame.addHeader("receipt", std::to_string(receiptCounter));
                receiptToStomp[std::to_string(receiptCounter)] = frame;
                channelToId.erase(channel);
                idToChannel.erase(id);
                receiptCounter++;
                isValidCommand = true;
            } else {
                std::cout << "Can't exit because you are not subscribed to " << channel << std::endl;
                return;
            }
        } else if (words[0] == "report") {
            if (!connected) {
                std::cout << "Please login first" << std::endl;
                return;
            }

            try {
                names_and_events parsed = parseEventsFile(words[1]);
                std::string gameName = parsed.team_a_name + "_" + parsed.team_b_name;
                if (channelToId.find(gameName) == channelToId.end()) {
                    std::cout << "You have not joined " << gameName << " yet. Can't send report." << std::endl;
                    return;
                }

                isValidCommand = true;

                for (const auto& event : parsed.events) {
                    gameUpdates[gameName][username].push_back(event);

                    std::string body = "user: " + username + "\n";
                    body += "team a: " + event.get_team_a_name() + "\n";
                    body += "team b: " + event.get_team_b_name() + "\n";
                    body += "event name: " + event.get_name() + "\n";
                    body += "time: " + std::to_string(event.get_time()) + "\n";
                    body += "general game updates:\n";
                    for (const auto& update : event.get_game_updates()) {
                        body += "    " + update.first + ": " + update.second + "\n";
                    }
                    body += "team a updates:\n";
                    for (const auto& update : event.get_team_a_updates()) {
                        body += "    " + update.first + ": " + update.second + "\n";
                    }
                    body += "team b updates:\n";
                    for (const auto& update : event.get_team_b_updates()) {
                        body += "    " + update.first + ": " + update.second + "\n";
                    }
                    body += "description:\n" + event.get_description();

                    auto& eventsList = gameUpdates[gameName][username];
                    std::sort(eventsList.begin(), eventsList.end(), [](const Event& a, const Event& b) {
                        return a.get_time() < b.get_time();
                    });

                    frame.setType("SEND");
                    frame.addHeader("destination", gameName);
                    frame.setBody(body);

                    std::cout << "--------------STOMP-SEND--------------" << std::endl;
                    std::cout << frame.toString() << std::endl;
                    std::cout << "--------------STOMP-SEND--------------" << std::endl;

                    if (!networkClient.sendFrame(frame.toString())) {
                        std::cout << "Disconnected. Failed to handleUserInput frame." << std::endl;
                        return;
                    }
                }

                this->report = true;
            } catch (const nlohmann::json::parse_error& e) {
                std::cout << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::cout << e.what() << std::endl;
            }
        }
    } else if (len >= 1) {
        if (words[0] == "logout") {
            if (!connected) {
                std::cout << "Already logged out" << std::endl;
                return;
            }

            frame.setType("DISCONNECT");
            frame.addHeader("receipt", std::to_string(receiptCounter));
            receiptToStomp[std::to_string(receiptCounter)] = frame;
            receiptCounter++;
            isValidCommand = true;
        }
    }

    if (isValidCommand && !report && !networkClient.sendFrame(frame.toString())) {
        std::cout << "Failed to send frame to server. Unexpected Issue" << std::endl;
        // std::cout << "Disconnected. Failed to handleUserInput frame." << std::endl;
        std::cout << "--------------STOMP-SEND--------------" << std::endl;
        std::cout << frame.toString() << std::endl;
        std::cout << "--------------STOMP-SEND--------------" << std::endl;
        return;
    } else if (!isValidCommand) {
        std::cout << "Please enter a valid command" << std::endl;
    }

    this->report = false;
}