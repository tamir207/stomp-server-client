#include "../include/StompProtocol.h"
#include "../include/Frame.h"
#include "../include/Utils.h"
#include "../include/event.h"

StompProtocol::StompProtocol()
    : connectionHandler(nullptr),
      socketListener(nullptr),
      subIDCounter(1),
      receiptCounter(1),
      channelToId(),
      idToChannel(),
      connected(false) { }

StompProtocol::~StompProtocol() {
    delete connectionHandler;
    delete socketListener;
}

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
    }

    return false;

    // socketListener->stop();
    // connectionHandler->close();
}

void StompProtocol::handleUserInput(std::string command) {
    Frame frame;
    std::vector<std::string> words = Utils::split(command, ' ');
    size_t len = words.size();

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
                connectionHandler = new ConnectionHandler(host, shortPort);
                if (!connectionHandler->connect()) {
                    std::cout << "Could not connect to server\n" << std::endl;
                }

                connected = true;
                socketListener = new SocketListener(*connectionHandler, [this](std::string msg) {
                    return this->handleServerInput(msg);
                });

                frame.setType("CONNECT");
                frame.addHeader("accept-version", "1.2");
                frame.addHeader("host", host);
                frame.addHeader("login", username);
                frame.addHeader("passcode", passcode);
                frame.addHeader("receipt", "123");
            }
        }
    } else if (len >= 2) {
        if (words[0] == "join") {
            std::string channel = words[1];
            if (channelToId.find(channel) == channelToId.end()) {
                frame.setType("SUBSCRIBE");
                frame.addHeader("destination", channel);
                frame.addHeader("id", std::to_string(subIDCounter));
                frame.addHeader("receipt", std::to_string(receiptCounter));

                channelToId[channel] = subIDCounter;
                idToChannel[subIDCounter] = channel;

                subIDCounter++;
                receiptCounter++;
            } else {
                std::cout << "you are already subscribed to " << channel << std::endl;
            }
        } else if (words[0] == "exit") {
            std::string channel = words[1];
            auto ind = channelToId.find(channel);
            if (ind != channelToId.end()) {
                int id = ind->second;
                frame.setType("UNSUBSCRIBE");
                frame.addHeader("id", std::to_string(id));
                frame.addHeader("receipt", std::to_string(receiptCounter));

                channelToId.erase(channel);
                idToChannel.erase(id);

                receiptCounter++;
            } else {
                std::cout << "you are NOT subscribed to " << channel << std::endl;
            }
        }
        else if (words[0] == "report") {
            names_and_events parsed = parseEventsFile(words[1]);
        }
    } else if (len >= 1) {
        if (words[0] == "logout") {
            frame.setType("DISCONNECT");
            frame.addHeader("receipt", std::to_string(receiptCounter));
            receiptCounter++;
        }
    }

    if (!connectionHandler->sendFrameAscii(frame.toString(), '\0')) {
        std::cout << "Disconnected. Failed to handleUserInput frame." << std::endl;
        return;
    }

    if (command != "") {
        std::cout << "--------------STOMP-SEND--------------" << std::endl;
        std::cout << frame.toString() << std::endl;
        std::cout << "--------------STOMP-SEND--------------" << std::endl;
    }
}

// 	connectionHandler()
//  socketListener(connectionHandler, [this](std::string msg) { this->handleServerInput(msg); })