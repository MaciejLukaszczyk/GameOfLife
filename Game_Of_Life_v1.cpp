#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;

class GameOfLife {
public:
    GameOfLife(int rows, int cols) : rows(rows), cols(cols) {
        board.resize(rows, vector<bool>(cols, false));
    }

    void setCell(int row, int col, bool alive) {
        if (row >= 0 && row < rows && col >= 0 && col < cols) {
            board[row][col] = alive;
        }
    }

    void display() const {
        system("clear"); // Use "cls" on Windows
        for (const auto& row : board) {
            for (bool cell : row) {
                cout << (cell ? 'O' : '.') << ' ';
            }
            cout << endl;
        }
    }

    void update() {
        vector<vector<bool>> newBoard = board;

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                int aliveNeighbors = countAliveNeighbors(i, j);
                if (board[i][j]) {
                    newBoard[i][j] = (aliveNeighbors == 2 || aliveNeighbors == 3);
                } else {
                    newBoard[i][j] = (aliveNeighbors == 3);
                }
            }
        }

        board = newBoard;
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
    vector<vector<bool>> board;

    int countAliveNeighbors(int row, int col) const {
        int count = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (i == 0 && j == 0) continue; // Skip the cell itself
                int newRow = row + i;
                int newCol = col + j;
                if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
                    count += board[newRow][newCol];
                }
            }
        }
        return count;
    }
};

int main() {
    int rows, cols;
    cout << "Enter number of rows: ";
    cin >> rows;
    cout << "Enter number of columns: ";
    cin >> cols;

    GameOfLife game(rows, cols);

    cout << "Enter initial state (row col alive(1)/dead(0), -1 to end):" << endl;
    while (true) {
        int r, c, alive;
        cin >> r;
        if (r == -1) break;
        cin >> c >> alive;
        game.setCell(r, c, alive == 1);
    }

    int iterations, delay;
    cout << "Enter number of iterations: ";
    cin >> iterations;
    cout << "Enter delay in milliseconds: ";
    cin >> delay;

    game.run(iterations, delay);

    return 0;
}