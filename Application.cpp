//
// Created by mluka on 17.05.2025.
//

#include "include/Application.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>

void Application::run() {
    try {
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        // Input validation
        size_t rows, cols;
        std::cout << "Enter rows and columns: ";
        if (!(std::cin >> rows >> cols) || rows == 0 || cols == 0) {
            throw std::invalid_argument("Invalid grid dimensions");
        }

        GameOfLife game(rows, cols);

        // Initialization
        char choice;
        std::cout << "Randomize (y/n)? ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw std::runtime_error("Invalid input format");
        }

        if (choice == 'y' || choice == 'Y') {
            game.randomize();
        } else {
            std::cout << "Enter cells (row col alive) [-1 to end]:\n";
            int r, c, a;
            while (true) {
                if (!(std::cin >> r) || r == -1) break;
                if (!(std::cin >> c >> a)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cerr << "Invalid input format, try again\n";
                    continue;
                }

                try {
                    if (r < 0 || c < 0) throw std::out_of_range("Negative coordinates");
                    game.setCell(static_cast<size_t>(r), static_cast<size_t>(c), a != 0);
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << " - skipping cell\n";
                }
            }
        }

        // Simulation parameters
        size_t iterations;
        int delay;
        std::cout << "Iterations and delay (ms): ";
        if (!(std::cin >> iterations >> delay)) {
            throw std::invalid_argument("Invalid simulation parameters");
        }

        // Network and simulation loop
        try {
            NetworkManager nm("127.0.0.1", 12345);
            nm.connect();

            for (size_t i = 0; i < iterations; ++i) {
                try {
                    nm.send(game.serialize());
                } catch (const std::exception& e) {
                    std::cerr << "Network error: " << e.what() << " - continuing simulation\n";
                }

                game.update();
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            }
        } catch (const std::exception& e) {
            std::cerr << "Network initialization failed: " << e.what() << " - running offline\n";
            for (size_t i = 0; i < iterations; ++i) {
                game.update();
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    }
}