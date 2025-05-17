//
// Created by mluka on 17.05.2025.
//

#include "include/NetworkManager.h"
#include <iostream>
#include <stdexcept>
#include <ws2tcpip.h>
#include <cstdint>
#pragma comment(lib, "ws2_32.lib")

NetworkManager::NetworkManager(const std::string& address, uint16_t port) {
    if (WSAStartup(MAKEWORD(2,2), &wsaData_) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }

    serverAddr_.sin_family = AF_INET;
    serverAddr_.sin_port = htons(port);
    inet_pton(AF_INET, address.c_str(), &serverAddr_.sin_addr);
}

NetworkManager::~NetworkManager() {
    disconnect();
    WSACleanup();
}

void NetworkManager::connect() {
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ == INVALID_SOCKET) {
        throw std::runtime_error("Socket creation failed");
    }

    if (::connect(socket_, reinterpret_cast<sockaddr*>(&serverAddr_), sizeof(serverAddr_)) == SOCKET_ERROR) {
        closesocket(socket_);
        throw std::runtime_error("Connection failed");
    }
    isConnected_ = true;
}

void NetworkManager::send(const std::string& data) {
    if (!isConnected_) throw std::runtime_error("Not connected");

    int result = ::send(socket_, data.c_str(), static_cast<int>(data.size()), 0);
    if (result == SOCKET_ERROR) {
        throw std::runtime_error("Send failed");
    }
}

void NetworkManager::disconnect() {
    if (isConnected_) {
        closesocket(socket_);
        isConnected_ = false;
    }
}