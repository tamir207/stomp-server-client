#pragma once
#include "../include/ConnectionHandler.h"
#include <iostream>
#include <thread>
#include <string>

class SocketListener
{
private:
	std::thread read_thread;
	ConnectionHandler &handler;
	std::function<void(std::string)> process_func;
	bool should_terminate;

public:	
	SocketListener(ConnectionHandler &ch, std::function<void(std::string)> process);

	// Run the reader thread only after you called ConnectionHandler.connect()
	void run();

	// Should set shouldTerminate to true and stop the thread. Should be called before closing the socket in ConnectionHandler
	void stop();
};