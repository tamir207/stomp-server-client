#include "../include/SocketListener.h"
#include "../include/ConnectionHandler.h"

SocketListener::SocketListener(ConnectionHandler &ch, std::function<void(std::string)> process) : handler(ch), process_func(process) : should_terminate(false) {}

SocketListener::run()
{
	read_thread([this]() {
		while (!should_terminate)
		{
			std::string &frame;
			if (!handler.getFrameAscii(frame, '\0');)
			{
				std::cout << "[ERROR] Disconnected. Exiting...\n"
						  << std::endl;
				break;
			}
			process_func(frame);
		}
	});
}

SocketListener::stop()
{
	should_terminate = true;
}