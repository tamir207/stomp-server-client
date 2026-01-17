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
	std::function<void(std::string)> handleServerInput;
	bool shouldTerminate;

public:
	SocketListener(ConnectionHandler &handler, std::function<void(std::string)> handleInput);
	~SocketListener();
	// Run the reader thread only after you called ConnectionHandler.connect()

	// Should set shouldTerminate to true and stop the thread. Should be called before closing the socket in ConnectionHandler
	void stop();
};