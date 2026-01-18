#pragma once
#include "./ConnectionHandler.h"
#include <atomic>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

class NetworkClient {
private:
    ConnectionHandler* handler;
    std::thread readThread;
    std::function<bool(std::string)> messageCallback;
    std::atomic<bool> isConnected;

    void runReadLoop();

public:
    NetworkClient();
    ~NetworkClient();

    void disconnect();

    bool connect(std::string host, short port, std::function<bool(std::string)> callback);

    bool sendFrame(const std::string& frameAscii);

    bool getIsConnected() const { return isConnected; }
};