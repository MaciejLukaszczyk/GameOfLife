//
// Created by mluka on 17.05.2025.
//

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H



#pragma once
#include <winsock2.h>
#include <string>
#include <cstdint>

class NetworkManager {
public:
    NetworkManager(const std::string& address, uint16_t port);
    ~NetworkManager();

    void connect();
    void send(const std::string& data);
    void disconnect();

private:
    SOCKET socket_ = INVALID_SOCKET;
    sockaddr_in serverAddr_{};
    bool isConnected_ = false;
    WSADATA wsaData_{};
};



#endif //NETWORKMANAGER_H
