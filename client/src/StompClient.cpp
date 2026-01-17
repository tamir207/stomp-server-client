#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"
#include "../include/Frame.h"
#include <string>

int main(int argc, char *argv[])
{

    // if (argc < 3)
    // {
    //     std::cerr << "Usage: " << argv[0] << " host port" << std::endl
    //               << std::endl;
    //     return -1;
    // }
    // std::string host = argv[1];
    // short port = atoi(argv[2]);

    StompProtocol protocol;

    // if (!connectionHandler.connect())
    // {
    //     std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
    //     return 1;
    // }

    while (1)
    {
        std::string input;
        std::cout << "Enter command -> " << std::flush;
        std::getline(std::cin, input);
        protocol.handleUserInput(input);
        //std::cout << "The input is: " << input << std::endl;
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
    //     std::cout << "Disconnected. Failed to handleUserInput frame." << std::endl;
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
