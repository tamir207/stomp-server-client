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
      report(false),
      username() { }

StompProtocol::~StompProtocol() { networkClient.disconnect(); }

bool StompProtocol::handleServerInput(std::string msg) {
    Frame frame = Frame(msg);
    std::cout << "Frame body FIRSTTTTTTTTT: " << frame.getBody() << std::endl;

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
            std::cout << "Frame body: " << frame.getBody() << std::endl;
            game.addEvent(frame.getBody());
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
            games.clear();
            return true;
        } else if (receiptAttachedFrame.getType() == "SUBSCRIBE") {
            std::string joined_channel_no_slash = receiptAttachedFrame.getHeaderValue("destination").substr(1);
            std::cout << "Joined channel " << joined_channel_no_slash << std::endl;
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
            std::string gameName = "/" + words[1];
            std::string userName = words[2];
            std::string filePath = words[3];
            std::cout << "Game name: " << gameName << std::endl;

            // for (const auto& [gameName, gameObj] : games) {
            //     std::cout << "Game key: " << gameName << std::endl;
            // }
            bool ans = games.find(gameName) != games.end();
            std::cout << "Should be true: " << ans << std::endl;

            if (games.find(gameName) == games.end()) {
                std::cout << "Can't summrize, you are not registered to " << gameName << ans << std::endl;
                return;
            }

            if (games.find(gameName) != games.end()) {
                std::cout << "Found game! " << gameName << " Username: " << userName << std::endl;
                std::string summaryOutput = games.at(gameName).summarize(userName);
                std::cout << "********************* summary output: **************************\n"
                          << summaryOutput << std::endl;

                if (summaryOutput.empty()) {
                    std::cout << "The user " << userName << " has not reported on the game " << gameName << std::endl;
                } else {
                    if (Utils::writeStringToFile(filePath, summaryOutput))
                        std::cout << "Write successful\n";
                    else
                        std::cout << "Write failed\n";

                    std::ofstream fileStream(filePath);
                    if (fileStream.is_open()) {
                        fileStream << summaryOutput;
                        fileStream.close();
                    }
                }
            }

            return;
        }
    } else if (len >= 2) {
        if (words[0] == "join") {
            if (!connected) {
                std::cout << "Please login first" << std::endl;
                return;
            }

            std::string channel = "/" + words[1];
            if (channelToId.find(channel) == channelToId.end() && connected) {
                frame.setType("SUBSCRIBE");
                frame.addHeader("destination", channel);
                frame.addHeader("id", std::to_string(subIDCounter));
                frame.addHeader("receipt", std::to_string(receiptCounter));
                channelToId[channel] = subIDCounter;
                idToChannel[subIDCounter] = channel;
                receiptToStomp[std::to_string(receiptCounter)] = frame;
                Game newGame = Game(channel);
                games.emplace(channel, Game(newGame));
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

            std::string channel = "/" + words[1];
            auto ind = channelToId.find(channel);
            if (ind != channelToId.end()) {
                int id = ind->second;
                frame.setType("UNSUBSCRIBE");
                frame.addHeader("id", std::to_string(id));
                frame.addHeader("receipt", std::to_string(receiptCounter));
                receiptToStomp[std::to_string(receiptCounter)] = frame;
                games.erase(channel);
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
                std::string gameName = "/" + parsed.team_a_name + "_" + parsed.team_b_name;
                if (channelToId.find(gameName) == channelToId.end()) {
                    std::cout << "You have not joined " << gameName << " yet. Can't send report." << std::endl;
                    return;
                }

                isValidCommand = true;
                for (Event& event : parsed.events) {
                    event.set_username(username);
                    Frame msgFrame;
                    msgFrame.setType("SEND");
                    msgFrame.addHeader("destination", gameName);
                    msgFrame.addHeader("filename", words[1]);
                    msgFrame.setBody(event.toString());
                    if (!networkClient.sendFrame(msgFrame.toString())) {
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