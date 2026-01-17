#include "../include/StompProtocol.h"

StompProtocol::StompProtocol() : 
    connection_handler(nullptr),
    encoder(),
    socket_listener(nullptr) 
{
}

void StompProtocol::process(std::string msg) {
    
}

void StompProtocol::send(std::string command){
    std::string stompMessage = encoder.encode(command);
    std::cout << "STOMP--------------:\n\n" << stompMessage << std::endl;
}

// 	connectionHandler()
//  socket_listener(connectionHandler, [this](std::string msg) { this->process(msg); })