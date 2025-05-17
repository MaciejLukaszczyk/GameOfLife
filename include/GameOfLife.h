//
// Created by mluka on 17.05.2025.
//

#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H



#pragma once
#include <vector>
#include <string>
#include <array>
#include <stdexcept>

class GameOfLife {
public:
    GameOfLife(size_t rows, size_t cols);

    void setCell(size_t row, size_t col, bool alive);
    void randomize();
    void update();
    std::string serialize() const;

    size_t getRows() const noexcept { return rows_; }
    size_t getCols() const noexcept { return cols_; }

private:
    bool isValidCell(int row, int col) const noexcept;
    int countAliveNeighbors(size_t row, size_t col) const;

    size_t rows_;
    size_t cols_;
    std::vector<std::vector<bool>> board_;
};
#endif //GAMEOFLIFE_H
