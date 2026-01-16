#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl
                  << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);

    ConnectionHandler connectionHandler(host, port);
    StompProtocol protocol(connectionHandler);
    protocol.start();
    // if (!connectionHandler.connect())
    // {
    //     std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
    //     return 1;
    // }

    while (1)
    {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);

        std::string line = "CONNECT\nreceipt: 12\n";
        line += '\0';
        int len = line.length();
        if (!connectionHandler.sendLine(line))
        {
            std::cout << "Disconnected. Exiting...\n"
                      << std::endl;
            break;
        }
        std::cout << "Sent " << len + 1 << " bytes to server" << std::endl;

        std::string answer;
        if (!connectionHandler.getLine(answer))
        {
            std::cout << "Disconnected. Exiting...\n"
                      << std::endl;
            break;
        }

        len = answer.length();
        answer.resize(len - 1);
        std::cout << "Reply: " << answer << " " << len << " bytes " << std::endl
                  << std::endl;
        if (answer == "bye")
        {
            std::cout << "Exiting...\n"
                      << std::endl;
            break;
        }
    }
    return 0;

    // if (argc < 3)
    // {
    //     std::cerr << "Usage: " << argv[0] << " host port" << std::endl;
    //     return -1;
    // }

    // std::string host = argv[1];
    // short port = atoi(argv[2]);

    // ConnectionHandler connectionHandler(host, port);
    // if (!connectionHandler.connect())
    // {
    //     std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
    //     return 1;
    // }

    // std::cout << "Connected to server. Sending CONNECT frame..." << std::endl;

    // std::string connectFrame = "CONNECT\naccept-version:1.2\nreceipt:32\nhost:stomp.cs.bgu.ac.il\nlogin:tamir\npasscode:spl\n\n";

    // if (!connectionHandler.sendFrameAscii(connectFrame, '\0'))
    // {
    //     std::cout << "Disconnected. Failed to send frame." << std::endl;
    //     return 1;
    // }

    // std::string answer;
    // if (!connectionHandler.getFrameAscii(answer, '\0'))
    // {
    //     std::cout << "Disconnected. Failed to receive frame." << std::endl;
    //     return 1;
    // }

    // std::cout << "Server response:\n"
    //           << answer << std::endl;

    // std::string answer2;
    // if (!connectionHandler.getFrameAscii(answer2, '\0'))
    // {
    //     std::cout << "Disconnected. Failed to receive frame." << std::endl;
    //     return 1;
    // }

    // std::cout << "Server response:" << answer2 << std::endl;

    // std::string disconnect = "DISCONNECT\nreceipt:13\n\n";
    // if (!connectionHandler.sendFrameAscii(disconnect, '\0'))
    // {
    //     std::cout << "Disconnected. Failed to receive frame." << std::endl;
    //     return 1;
    // }

    // std::cout << "Server response:" << disconnect << std::endl;
    // std::string answer3;
    // if (!connectionHandler.getFrameAscii(answer3, '\0'))
    // {
    //     std::cout << "Disconnected. Failed to receive frame." << std::endl;
    //     return 1;
    // }

    // std::cout << "Server response:\n"
    //           << answer3 << std::endl;

    // return 0;
}