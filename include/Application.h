#ifndef APPLICATION_H
#define APPLICATION_H

#pragma once

#include "GameOfLife.h"
#include "NetworkManager.h"
#include <chrono>
#include <thread>
#include <cstddef>

class Application {
public:
    void run();

private:
    // Etapy działania aplikacji:
    void init();            // Pobranie wymiarów i przygotowanie losowości
    void setupGame();       // Wczytanie planszy życia (losowo lub ręcznie)
    void runSimulation();   // Przeprowadzenie symulacji (online lub offline)
    void finalize();        // Końcowy komunikat

    // Pola konfiguracyjne i stan symulacji
    size_t rows = 10;
    //size_t rows{};
    size_t cols = 10;
    //size_t cols{};
    size_t iterations = 1000;
    //size_t iterations{};
    int delay = 500;
    //int delay{};
    //GameOfLife game;
    GameOfLife game{50, 50};
};

#endif // APPLICATION_H

