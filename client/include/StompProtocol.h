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
int receiptCounter;
std::map<std::string, int> channelToId;
std::map<int, std::string> idToChannel;
bool connected;
public:

StompProtocol();
~StompProtocol();

// Should process server response and return true iff it should stop reading
bool handleServerInput(std::string msg);

void handleUserInput(std::string command);
};