#include "../include/StompProtocol.h"
#include "../include/Frame.h"

StompProtocol::StompProtocol() : 
    connection_handler(nullptr),
    encoder(),
    socket_listener(nullptr) 
{
}

void StompProtocol::process(std::string msg) {
    Frame frame = Frame(msg);
    if (frame.getType() == "CONNECTED") {

    }
}

void StompProtocol::send(std::string command){
    std::string stompMessage = encoder.encode(command);
    std::cout << "STOMP--------------:\n\n" << stompMessage << std::endl;
}

// 	connectionHandler()
//  socket_listener(connectionHandler, [this](std::string msg) { this->process(msg); })