#pragma once
#include "../include/ConnectionHandler.h"
#include <iostream>
#include <thread>
#include <string>

class SocketListener
{
private:
	std::thread readThread;
	ConnectionHandler &handler;
	std::function<bool(std::string)> handleServerInput;
	bool shouldTerminate;

public:
	SocketListener(ConnectionHandler &handler, std::function<bool(std::string)> handleInput);
	~SocketListener();
	// Run the reader thread only after you called ConnectionHandler.connect()
};