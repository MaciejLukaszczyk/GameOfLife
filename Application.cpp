#include "include/Application.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <thread>
#include <chrono>

void Application::run() {
    try {
        init();
        setupGame();
        runSimulation();
        finalize();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    }
}

void Application::init() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "Enter rows and columns: ";
    if (!(std::cin >> rows >> cols) || rows == 0 || cols == 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw std::invalid_argument("[ERROR] Invalid grid dimensions.");
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Application::setupGame() {
    game = GameOfLife(rows, cols);

    char choice;
    std::cout << "Randomize (y/n)? ";
    if (!(std::cin >> std::ws >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw std::invalid_argument("[ERROR] Invalid input format.");
    }

    if (choice == 'y' || choice == 'Y') {
        game.randomize();
    } else {
        std::cout << "Enter cells (row col alive) [-1 to end]:\n";
        int r, c, a;
        while (true) {
            if (!(std::cin >> r)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Invalid input format, try again\n";
                continue;
            }
            if (r == -1) break;

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

    std::cout << "Iterations and delay (ms): ";
    if (!(std::cin >> iterations >> delay)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw std::invalid_argument("[ERROR] Invalid simulation parameters.");
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Application::runSimulation() {
    try {
        NetworkManager nm("127.0.0.1", 12345);
        nm.connect();
        std::cout << "[DEBUG] Server connected to client.\n";

        for (size_t i = 0; i < iterations; ++i) {
            try {
                nm.send(game.serialize());
                std::cout << "[DEBUG] Sent iteration " << i << "\n";
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
}

void Application::finalize() {
    std::cout << "Simulation ended. Press Enter to close...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}
