#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/NetworkClient.h"
#include "../include/Frame.h"
#include "../include/event.h"
#include "../include/Game.h"
#include "NetworkClient.h"
#include <string>
#include <iostream>

class StompProtocol
{
private:
int subIDCounter;
int receiptCounter;
std::map<std::string, int> channelToId;
std::map<int, std::string> idToChannel;
std::string username;
bool connected;
bool report;
// <receipt_id, frameType>
std::map<std::string, Frame> receiptToStomp;
//<game, <event-user, events>>
std::map<std::string, Game> games; // map<game_name, Game object>
NetworkClient networkClient;
public:

StompProtocol();
~StompProtocol();
// Should process server response and return true iff it should stop reading
bool handleServerInput(std::string msg);

void handleUserInput(std::string command);
};