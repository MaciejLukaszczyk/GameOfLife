//
// Created by mluka on 17.05.2025.
//

#include "include/NetworkManager.h"
#include <iostream>
#include <stdexcept>
#include <ws2tcpip.h>
#include <cstdint>
#include <thread>
#include <chrono>

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

    BOOL opt = TRUE;
    setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    if (bind(socket_, reinterpret_cast<sockaddr*>(&serverAddr_), sizeof(serverAddr_)) == SOCKET_ERROR) {
        closesocket(socket_);
        throw std::runtime_error("Bind failed");
    }

    if (listen(socket_, 1) == SOCKET_ERROR) {
        closesocket(socket_);
        throw std::runtime_error("Listen failed");
    }

    std::cout << "[SERVER] Listening on port " << ntohs(serverAddr_.sin_port) << "...\n";

    SOCKET clientSocket = INVALID_SOCKET;
    while (clientSocket == INVALID_SOCKET) {
        clientSocket = accept(socket_, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "[SERVER] Waiting for client to connect...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    std::cout << "[SERVER] Client connected!\n";

    closesocket(socket_);
    socket_ = clientSocket;
    isConnected_ = true;
}



void NetworkManager::send(const std::string& data) {
    if (!isConnected_) throw std::runtime_error("Not connected");

    std::string message = data + '\n';  // Dodaj nową linię jako znacznik końca ramki
    int result = ::send(socket_, message.c_str(), static_cast<int>(message.size()), 0);
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