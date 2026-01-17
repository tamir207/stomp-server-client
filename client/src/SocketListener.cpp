#include "../include/SocketListener.h"
#include "../include/ConnectionHandler.h"

SocketListener::SocketListener(ConnectionHandler &ch, std::function<bool(std::string)> handleInput)
	: handler(ch),
	  handleServerInput(handleInput),
	  shouldTerminate(false)
{
	std::cout << "Starting thread" << std::endl;
	readThread = std::thread([this]() {
		std::cout << "Thread started"  << std::endl;
		while (1)
		{
			std::cout << "Running loop " << std::endl;
			std::string frame;
			if (!handler.getFrameAscii(frame, '\0'))
			{
				std::cout << "[ERROR] Disconnected. Exiting...\n" << std::endl;
				break;
			}
			std::cout << "Ended loop" << std::endl;
			bool shouldTerminate = handleServerInput(frame);
			if (shouldTerminate == false) {
				break;
			}
		} 
		
		std::cout << "0942875-908-50798134=6981=34689=98=08=9" << std::endl;

	});
}

SocketListener::~SocketListener()
{
	if (readThread.joinable())
	{
		readThread.join();
	}
}