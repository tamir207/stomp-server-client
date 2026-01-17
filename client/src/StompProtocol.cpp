#include "../include/StompProtocol.h"
#include "../include/Frame.h"
#include "../include/Utils.h"

StompProtocol::StompProtocol() : connectionHandler(nullptr),
                                 socketListener(nullptr),
                                 subIDCounter(1),
                                 channelToId(),
                                 idToChannel()
{
}

StompProtocol::~StompProtocol()
{
    delete connectionHandler;
    delete socketListener;
}

void StompProtocol::handleServerInput(std::string msg)
{
    Frame frame = Frame(msg);
    std::cout << frame.toString() << std::endl;

    if (frame.getType() == "CONNECTED")
    {
        std::cout << frame.toString() << std::endl;
    }
}

void StompProtocol::handleUserInput(std::string command)
{
    Frame frame;
    std::vector<std::string> words = Utils::split(command, ' ');
    size_t len = words.size();

    if (len >= 4)
    {
        if (words[0] == "login")
        {
            size_t colonPos = words[1].find(':');
            if (colonPos != std::string::npos)
            {
                std::string host = words[1].substr(0, colonPos);
                std::string port = words[1].substr(colonPos + 1);
                std::string username = words[2];
                std::string passcode = words[3];
                
                std::cout << "-------1111111111111111-------\n\n" << std::endl;
                short shortPort = static_cast<short>(std::stoi(port));
                std::cout << "-------222222222222222-------\n\n" << std::endl;

                connectionHandler = new ConnectionHandler(host, shortPort);
                if (!connectionHandler->connect())
                {
                    std::cout << "--------------\n\n" << std::endl;
                }
                socketListener = new SocketListener(*connectionHandler,
                                                    [this](std::string msg)
                                                    {
                                                        this->handleServerInput(msg);
                                                    });

                frame.setType("CONNECT");
                frame.addHeader("accept-version", "1.2");
                frame.addHeader("host", host);
                frame.addHeader("login", username);
                frame.addHeader("passcode", passcode);
            }
        }
    }
    else if (len >= 2)
    {
    }

    std::cout << "STOMP--------------\n\n" << frame.toString() << std::endl;
}

// 	connectionHandler()
//  socketListener(connectionHandler, [this](std::string msg) { this->handleServerInput(msg); })