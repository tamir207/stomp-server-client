#include "../include/SocketListener.h"
#include "../include/ConnectionHandler.h"

SocketListener::SocketListener(ConnectionHandler &ch, std::function<void(std::string)> handleInput) : handler(ch), handleServerInput(handleInput), shouldTerminate(false)
{
	readThread = std::thread([this]() {
		while (!shouldTerminate)
		{
			std::string frame;
			if (!handler.getFrameAscii(frame, '\0'))
			{
				std::cout << "[ERROR] Disconnected. Exiting...\n" << std::endl;
				break;
			}

			handleServerInput(frame);
		} 
	});
}

void SocketListener::stop()
{
	shouldTerminate = true;
}

SocketListener::~SocketListener() {
    stop(); 
    if (readThread.joinable()) {
        readThread.join();
    }
}