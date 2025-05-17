// === GameOfLife.h ===
#pragma once
#include <vector>
#include <string>
#include <array>
#include <cstdint>

class GameOfLife {
public:
    // Konstruktor inicjalizujący planszę o podanych wymiarach
    GameOfLife(size_t rows, size_t cols);

    // Ustawienie stanu konkretnej komórki
    void setCell(size_t row, size_t col, uint8_t state);

    // Losowe przypisanie stanów do wszystkich komórek
    void randomize();

    // Przejście do kolejnego stanu symulacji
    void update();

    // Serializacja planszy do formatu tekstowego
    std::string serialize() const;

private:
    size_t rows_, cols_;
    std::vector<std::vector<uint8_t>> board_;  // plansza z wieloma stanami

    // Sprawdza, czy współrzędne są w granicach planszy
    bool isValidCell(int row, int col) const noexcept;

    // Zlicza żywe sąsiednie komórki (stan > 0)
    int countAliveNeighbors(size_t row, size_t col) const;

    // Zlicza sąsiadów o określonych stanach (np. REPRODUCTIVE)
    int countNeighborsWithState(size_t row, size_t col, const std::vector<uint8_t>& states) const;
};

// Definicje stanów komórek:
// 0 - DEAD         (martwa)
// 1 - YOUNG        (dorastająca)
// 2 - REPRODUCTIVE (rozpłodowa)
// 3 - OLD_REPRO    (stara rozpłodowa)
// 4 - DYING        (umierająca)
