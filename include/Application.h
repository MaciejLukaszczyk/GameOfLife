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
    size_t rows{};
    size_t cols{};
    size_t iterations{};
    int delay{};
    GameOfLife game{0, 0};
};

#endif // APPLICATION_H

