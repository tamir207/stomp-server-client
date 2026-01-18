#include "../include/NetworkClient.h"

NetworkClient::NetworkClient()
    : handler(nullptr),
      isConnected(false) { }

NetworkClient::~NetworkClient() { disconnect(); }

bool NetworkClient::connect(std::string host, short port, std::function<bool(std::string)> callback) {
    if (isConnected)
        return false;

    disconnect();

    handler = new ConnectionHandler(host, port);
    if (!handler->connect()) {
        std::cerr << "NetworkClient: Could not connect to " << host << ":" << port << std::endl;
        delete handler;
        handler = nullptr;
        return false;
    }

    this->messageCallback = callback;
    this->isConnected = true;
    readThread = std::thread([this]() { runReadLoop(); });
    return true;
}

void NetworkClient::disconnect() {
    isConnected = false;

    if (handler != nullptr) {
        handler->close();
    }

    if (readThread.joinable()) {
        readThread.join();
    }

    if (handler != nullptr) {
        delete handler;
        handler = nullptr;
        std::cout << "NetworkClient: Disconnected and cleaned up." << std::endl;
    }
}

bool NetworkClient::sendFrame(const std::string& frameAscii) {
    if (!isConnected)
        return false;
    return handler->sendFrameAscii(frameAscii, '\0');
}

void NetworkClient::runReadLoop() {
    while (isConnected) {
        std::string frame;
        if (!handler->getFrameAscii(frame, '\0')) {
            std::cout << "NetworkClient: Server disconnected." << std::endl;
            break;
        }

        bool shouldTerminate = messageCallback(frame);
        if (shouldTerminate) {
            break;
        }
    }

    isConnected = false;
    if (handler != nullptr) {
        handler->close();
    }
}