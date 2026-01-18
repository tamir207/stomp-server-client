#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/NetworkClient.h"
#include "../include/Frame.h"
#include "../include/event.h"
#include "NetworkClient.h"
#include <string>
#include <iostream>

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
int subIDCounter;
int receiptCounter;
std::map<std::string, int> channelToId;
std::map<int, std::string> idToChannel;
bool connected;
// <receipt_id, frameType>
std::map<std::string, std::string> receiptToStomp;
//<game, <event-user, events>>
std::map<std::string, std::map<std::string, std::vector<Event>>> gameUpdates;
NetworkClient networkClient;
public:

StompProtocol();
~StompProtocol();
// Should process server response and return true iff it should stop reading
bool handleServerInput(std::string msg);

void handleUserInput(std::string command);
};