//
// Created by mluka on 17.05.2025.
//

#ifndef APPLICATION_H
#define APPLICATION_H



#pragma once
#include "GameOfLife.h"
#include "NetworkManager.h"
#include <chrono>
#include <thread>

class Application {
public:
    void run();

private:
    void configureGame(GameOfLife& game);
};



#endif //APPLICATION_H
