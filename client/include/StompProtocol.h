#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/SocketListener.h"
#include "../include/CommandEncoder.h"
#include <string>
#include <iostream>

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
ConnectionHandler* connection_handler;
CommandEncoder encoder;
SocketListener* socket_listener;
public:

StompProtocol();

// Should create runnable task for the thread to execute.
void process(std::string msg);


void send(std::string command);
};