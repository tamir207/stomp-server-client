#pragma once

#include "../include/ConnectionHandler.h"

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
const ConnectionHandler& connectionHandler;
std::thread myThread;
public:

StompProtocol(ConnectionHandler& newConnectionHandler);

// Should start the communication thread
void start();

// Should create runnable task for the thread to execute.
void process(std::string msg);


void send(std::string msg);

};
