#include "../include/StompProtocol.h"
#include "../include/Frame.h"
#include "../include/Utils.h"

StompProtocol::StompProtocol()
    : connectionHandler(nullptr),
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

bool StompProtocol::handleServerInput(std::string msg)
{
    Frame frame = Frame(msg);
    std::cout << "------------STOMP-RECEIVED------------" << std::endl;
    std::cout << msg << std::endl;
    std::cout << "------------STOMP-RECEIVED------------" << std::endl;
    if (frame.getType() == "CONNECTED")
    {
        std::cout << "Stopping client " << std::endl;
        return true;
    }
    else if (frame.getType() == "RECEIPT")
    {
    }

    return false;

    // socketListener->stop();
    // connectionHandler->close();
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
                short shortPort = static_cast<short>(std::stoi(port));
                connectionHandler = new ConnectionHandler(host, shortPort);
                if (!connectionHandler->connect())
                {
                    std::cout << "-------ERORRRRRRRRRRRRRRRRRRRRRRRRRRR-------\n\n"
                              << std::endl;
                }
                else
                {
                    std::cout << "-------successfull contecting-------\n\n";
                }

                socketListener = new SocketListener(*connectionHandler,
                                                    [this](std::string msg)
                                                    {
                                                        return this->handleServerInput(msg);
                                                    });

                frame.setType("CONNECT");
                frame.addHeader("accept-version", "1.2");
                frame.addHeader("host", host);
                frame.addHeader("login", username);
                frame.addHeader("passcode", passcode);
                frame.addHeader("receipt", "123");

                if (!connectionHandler->sendFrameAscii(frame.toString(), '\0'))
                {
                    std::cout << "Disconnected. Failed to handleUserInput frame." << std::endl;
                    return;
                }
            }
        }
    }
    else if (len >= 2)
    {
    }
    if (command != "")
    {
        std::cout << "--------------STOMP-SEND--------------" << std::endl;
        std::cout << frame.toString() << std::endl;
        std::cout << "--------------STOMP-SEND--------------" << std::endl;
    }
}

// 	connectionHandler()
//  socketListener(connectionHandler, [this](std::string msg) { this->handleServerInput(msg); })