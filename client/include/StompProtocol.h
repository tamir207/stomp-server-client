#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/SocketListener.h"
#include "../include/Frame.h"
#include "../include/event.h"
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
std::map<std::string, std::map<std::string, std::vector<Event>>> gameUpdates;//<game, <event-user, events>>
public:

StompProtocol();
~StompProtocol();

// Should process server response and return true iff it should stop reading
bool handleServerInput(std::string msg);

void handleUserInput(std::string command);
};