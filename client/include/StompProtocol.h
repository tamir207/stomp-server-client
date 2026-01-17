#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/SocketListener.h"
#include "../include/Frame.h"
#include <string>
#include <iostream>

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
ConnectionHandler* connectionHandler;
SocketListener* socketListener;
int subIDCounter;
std::map<std::string, int> channelToId;
std::map<int, std::string> idToChannel;
public:

StompProtocol();
~StompProtocol();

// Should create runnable task for the thread to execute.
void handleServerInput(std::string msg);

void handleUserInput(std::string command);
};