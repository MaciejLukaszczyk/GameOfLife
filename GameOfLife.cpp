// === GameOfLife.cpp ===
#include "include/GameOfLife.h"
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <ctime>

GameOfLife::GameOfLife(size_t rows, size_t cols)
    : rows_(rows), cols_(cols), board_(rows, std::vector<uint8_t>(cols, 0)) {
    if (rows == 0 || cols == 0) {
        throw std::invalid_argument("Grid dimensions must be positive");
    }
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void GameOfLife::setCell(size_t row, size_t col, uint8_t state) {
    if (row >= rows_ || col >= cols_) {
        throw std::out_of_range("Cell coordinates out of bounds");
    }
    board_[row][col] = state;
}

void GameOfLife::randomize() {
    for (auto& row : board_) {
        for (auto& cell : row) {
            cell = rand() % 5; // random state between 0 and 4
        }
    }
}

void GameOfLife::update() {
    std::vector<std::vector<uint8_t>> new_board = board_;

    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            int reproductive_neighbors = countNeighborsWithState(i, j, {2, 3});
            uint8_t current = board_[i][j];

            if (current == 0) { // DEAD
                if (reproductive_neighbors == 3) {
                    new_board[i][j] = 1; // born as YOUNG
                }
            } else if (current == 1) {
                new_board[i][j] = 2; // YOUNG -> REPRODUCTIVE
            } else if (current == 2) {
                new_board[i][j] = 3; // REPRODUCTIVE -> OLD_REPRO
            } else if (current == 3) {
                new_board[i][j] = 4; // OLD_REPRO -> DYING
            } else if (current == 4) {
                new_board[i][j] = 0; // DYING -> DEAD
            }
        }
    }

    board_ = std::move(new_board);
}

std::string GameOfLife::serialize() const {
    std::string data;
    data.reserve((cols_ + 1) * rows_ + 16);

    data += std::to_string(rows_) + ' ' + std::to_string(cols_) + '\n';

    for (const auto& row : board_) {
        for (uint8_t cell : row) {
            data += std::to_string(cell) + " ";
        }
        data += '\n';
    }
    data += "\n";

    return data;
}

bool GameOfLife::isValidCell(int row, int col) const noexcept {
    return row >= 0 && col >= 0 &&
           row < static_cast<int>(rows_) &&
           col < static_cast<int>(cols_);
}

int GameOfLife::countAliveNeighbors(size_t row, size_t col) const {
    static constexpr std::array<std::pair<int, int>, 6> even_directions = {
        {{-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, 0}, {1, 1}}
    };
    static constexpr std::array<std::pair<int, int>, 6> odd_directions = {
        {{-1, -1}, {-1, 0}, {0, -1}, {0, 1}, {1, -1}, {1, 0}}
    };

    const auto& directions = (row % 2 == 0) ? even_directions : odd_directions;
    int count = 0;

    for (const auto& [dr, dc] : directions) {
        int nr = static_cast<int>(row) + dr;
        int nc = static_cast<int>(col) + dc;
        if (isValidCell(nr, nc) && board_[nr][nc] > 0) {
            count++;
        }
    }

    return count;
}

int GameOfLife::countNeighborsWithState(size_t row, size_t col, const std::vector<uint8_t>& states) const {
    static constexpr std::array<std::pair<int, int>, 6> even_directions = {
        {{-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, 0}, {1, 1}}
    };
    static constexpr std::array<std::pair<int, int>, 6> odd_directions = {
        {{-1, -1}, {-1, 0}, {0, -1}, {0, 1}, {1, -1}, {1, 0}}
    };

    const auto& directions = (row % 2 == 0) ? even_directions : odd_directions;
    int count = 0;

    for (const auto& [dr, dc] : directions) {
        int nr = static_cast<int>(row) + dr;
        int nc = static_cast<int>(col) + dc;
        if (isValidCell(nr, nc)) {
            for (uint8_t s : states) {
                if (board_[nr][nc] == s) {
                    count++;
                    break;
                }
            }
        }
    }

    return count;
}
