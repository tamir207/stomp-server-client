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
            game.addEvent(frame.getBody());
        }
    } else if (frame.getType() == "RECEIPT") {
        Frame receiptAttachedFrame = receiptToStomp[frame.getHeaderValue("receipt-id")];
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
        } else if (receiptAttachedFrame.getType() == "UNSUBSCRIBE") {
            int subscriptionId = std::stoi(receiptAttachedFrame.getHeaderValue("id"));
            std::string exited_channel = idToChannel[subscriptionId];
            std::string exited_channel_no_slash = exited_channel.substr(1);
            games.erase(exited_channel);
            channelToId.erase(exited_channel);
            idToChannel.erase(subscriptionId);
            std::cout << "Exited channel " << exited_channel_no_slash << std::endl;
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
                std::cout << "\nThe client is already logged in, log out before trying again\n" << std::endl;
                return;
            }
            size_t colonPos = words[1].find(':');
            if (colonPos != std::string::npos) {
                std::string host = words[1].substr(0, colonPos);
                std::string port = words[1].substr(colonPos + 1);
                std::string username = words[2];
                std::string passcode = words[3];
                short shortPort = static_cast<short>(std::stoi(port));
                bool connected = networkClient.connect(host, shortPort, [this](std::string msg) {
                    return this->handleServerInput(msg);
                });

                if (!connected) {
                    std::cout << "\nCould not connect to server\n" << std::endl;
                    return;
                }

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
            bool ans = games.find(gameName) != games.end();
            if (games.find(gameName) == games.end()) {
                std::cout << "\nCan't summrize, you are not registered to " << gameName << ans << "\n" << std::endl;
                return;
            }

            if (games.find(gameName) != games.end()) {
                std::string summaryOutput = games.at(gameName).summarize(userName);
                if (summaryOutput.empty()) {
                    std::cout << "\nThe user " << userName << " has not reported on the game " << gameName.substr(1)
                              << "\n"
                              << std::endl;
                } else {
                    if (Utils::writeStringToFile(filePath, summaryOutput))
                        std::cout << "Write successful\n";
                    else
                        std::cout << "\nWrite failed\n";

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
                std::cout << "\nPlease login first\n" << std::endl;
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
                std::cout << "\nYou are already subscribed to " << channel << "\n" << std::endl;
            }
        } else if (words[0] == "exit") {
            if (!connected) {
                std::cout << "\nPlease login first\n" << std::endl;
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
                receiptCounter++;
                isValidCommand = true;
            } else {
                std::cout << "\nCan't exit because you are not subscribed to " << channel << "\n" << std::endl;
                return;
            }
        } else if (words[0] == "report") {
            if (!connected) {
                std::cout << "\nPlease login first\n" << std::endl;
                return;
            }

            try {
                names_and_events parsed = parseEventsFile(words[1]);
                std::string gameName = "/" + parsed.team_a_name + "_" + parsed.team_b_name;
                if (channelToId.find(gameName) == channelToId.end()) {
                    std::cout << "\nYou have not joined " << gameName << " yet. Can't send report.\n" << std::endl;
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
                std::cout << "\nAlready logged out\n" << std::endl;
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
        std::cout << "\nCould not connect to server\n" << std::endl;
        return;
    } else if (!isValidCommand) {
        std::cout << "\nPlease enter a valid command\n" << std::endl;
    }

    this->report = false;
}