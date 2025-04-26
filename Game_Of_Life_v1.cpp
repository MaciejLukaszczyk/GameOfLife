#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <optional>
#include <algorithm>
#include <ctime>

class GameOfLife {
public:
    GameOfLife(size_t rows, size_t cols) : rows(rows), cols(cols), board(rows, std::vector<bool>(cols, false)) {}

    void setCell(size_t row, size_t col, bool alive) {
        if (isValidCell(row, col)) {
            board[row][col] = alive;
        }
    }

    void randomize() {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                board[i][j] = rand() % 2; // Randomly set to 0 or 1
            }
        }
    }

    void display() const {
        system("cls"); // Use "cls" on Windows
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < i; ++j) {
                std::cout << " ";
            }
            for (size_t j = 0; j < cols; ++j) {
                std::cout << (board[i][j] ? 'O' : '.') << ' ';
            }
            std::cout << '\n';
        }
    }

    void update() {
        auto newBoard = board;

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                int aliveNeighbors = countAliveNeighbors(i, j);
                newBoard[i][j] = (board[i][j] && (aliveNeighbors == 2 || aliveNeighbors == 3)) ||
                                 (!board[i][j] && aliveNeighbors == 3);
            }
        }

        board = std::move(newBoard);
    }

    void run(size_t iterations, int delay) {
        for (size_t i = 0; i < iterations; ++i) {
            display();
            update();
            printIterationInfo(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
    }

private:
    size_t rows, cols;
    std::vector<std::vector<bool>> board;

    bool isValidCell(size_t row, size_t col) const {
        return row < rows && col < cols;
    }

    int countAliveNeighbors(size_t row, size_t col) const {
        int count = 0;
        const std::vector<std::pair<int, int>> directions = {
            {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, 0}, {1, 1}
        };

        for (const auto& [dr, dc] : directions) {
            if (isValidCell(row + dr, col + dc)) {
                count += board[row + dr][col + dc];
            }
        }
        return count;
    }

    void printIterationInfo(size_t iteration) const {
        int livingCells = 0;
        int deadCells = 0;

        for (const auto& row : board) {
            for (bool cell : row) {
                if (cell) {
                    livingCells++;
                } else {
                    deadCells++;
                }
            }
        }

        std::cout << "Iteration: " << iteration + 1
                  << ", Living Cells: " << livingCells
                  << ", Dead Cells: " << deadCells << '\n';
    }
};

int main() {
    srand(static_cast<unsigned int>(time(0))); // Seed for random number generation

    size_t rows, cols;
    std::cout << "Enter number of rows: ";
    std::cin >> rows;
    std::cout << "Enter number of columns: ";
    std::cin >> cols;

    GameOfLife game(rows, cols);

    char choice;
    std::cout << "Do you want to randomize the initial state? (y/n): ";
    std::cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        game.randomize();
    } else {
        std::cout << "Enter initial state (row col alive(1)/dead (0), -1 to end):\n";
        while (true) {
            int r, c, alive;
            std::cin >> r;
            if (r == -1) break;
            std::cin >> c >> alive;
            game.setCell(static_cast<size_t>(r), static_cast<size_t>(c), alive == 1);
        }
    }

    size_t iterations;
    int delay;
    std::cout << "Enter number of iterations: ";
    std::cin >> iterations;
    std::cout << "Enter delay in milliseconds: ";
    std::cin >> delay;

    game.run(iterations, delay);

    return 0;
}