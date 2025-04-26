#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <optional>
#include <algorithm>

class GameOfLife {
public:
    GameOfLife(int rows, int cols) : rows(rows), cols(cols), board(rows, std::vector<bool>(cols, false)) {}

    void setCell(int row, int col, bool alive) {
        if (isValidCell(row, col)) {
            board[row][col] = alive;
        }
    }

    void display() const {
        system("cls"); // Use "cls" on Windows
        for (const auto& row : board) {
            for (bool cell : row) {
                std::cout << (cell ? 'O' : '.') << ' ';
            }
            std::cout << '\n';
        }
    }

    void update() {
        auto newBoard = board;

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                int aliveNeighbors = countAliveNeighbors(i, j);
                newBoard[i][j] = (board[i][j] && (aliveNeighbors == 2 || aliveNeighbors == 3)) ||
                                 (!board[i][j] && aliveNeighbors == 3);
            }
        }

        board = std::move(newBoard);
    }

    void run(int iterations, int delay) {
        for (int i = 0; i < iterations; ++i) {
            display();
            update();
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
    }

private:
    int rows, cols;
    std::vector<std::vector<bool>> board;

    bool isValidCell(int row, int col) const {
        return row >= 0 && row < rows && col >= 0 && col < cols;
    }

    int countAliveNeighbors(int row, int col) const {
        int count = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (i == 0 && j == 0) continue; // Skip the cell itself
                if (isValidCell(row + i, col + j)) {
                    count += board[row + i][col + j];
                }
            }
        }
        return count;
    }
};

int main() {
    int rows, cols;
    std::cout << "Enter number of rows: ";
    std::cin >> rows;
    std::cout << "Enter number of columns: ";
    std::cin >> cols;

    GameOfLife game(rows, cols);

    std::cout << "Enter initial state (row col alive(1)/dead(0), -1 to end):\n";
    while (true) {
        int r, c, alive;
        std::cin >> r;
        if (r == -1) break;
        std::cin >> c >> alive;
        game.setCell(r, c, alive == 1);
    }

    int iterations, delay;
    std::cout << "Enter number of iterations: ";
    std::cin >> iterations;
    std::cout << "Enter delay in milliseconds: ";
    std::cin >> delay;

    game.run(iterations, delay);

    return 0;
}