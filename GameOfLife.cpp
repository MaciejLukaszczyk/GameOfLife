//
// Created by mluka on 17.05.2025.
//

#include "include/GameOfLife.h"

#include <stdexcept>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstdint>
GameOfLife::GameOfLife(size_t rows, size_t cols)
    : rows_(rows), cols_(cols), board_(rows, std::vector<bool>(cols, false)) {
    if (rows == 0 || cols == 0) {
        throw std::invalid_argument("Grid dimensions must be positive");
    }
}

void GameOfLife::setCell(size_t row, size_t col, bool alive) {
    if (row >= rows_ || col >= cols_) {
        throw std::out_of_range("Cell coordinates (" +
                              std::to_string(row) + ", " +
                              std::to_string(col) + ") out of bounds");
    }
    board_[row][col] = alive;
}

void GameOfLife::randomize() {
    for (auto&& row : board_) {
        for (auto&& cell : row) {
            cell = rand() % 2 == 1;
        }
    }
}

void GameOfLife::update() {
    std::vector<std::vector<bool>> new_board = board_;

    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            const int alive_neighbors = countAliveNeighbors(i, j);
            new_board[i][j] = (board_[i][j] && (alive_neighbors == 2 || alive_neighbors == 3)) ||
                             (!board_[i][j] && alive_neighbors == 3);
        }
    }

    board_ = std::move(new_board);
}

std::string GameOfLife::serialize() const {
    std::string data;
    data.reserve((cols_ + 1) * rows_ + 16);  // Pre-allocate memory

    data += std::to_string(rows_) + ' ' + std::to_string(cols_) + '\n';

    for (const auto& row : board_) {
        for (const bool cell : row) {
            data += cell ? "1 " : "0 ";
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
        const int nr = static_cast<int>(row) + dr;
        const int nc = static_cast<int>(col) + dc;

        if (isValidCell(nr, nc)) {
            count += board_[nr][nc] ? 1 : 0;
        }
    }

    return count;
}