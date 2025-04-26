#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

class GameOfLife {
public:
    GameOfLife(int rows, int cols) : rows(rows), cols(cols), board(rows, std::vector<bool>(cols, false)) {}

    void setCell(int row, int col, bool alive) {
        if (isValidCell(row, col)) {
            board[row][col] = alive;
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

    std::string boardToString() const {
        std::string result = std::to_string(rows) + " " + std::to_string(cols) + "\n";
        for (const auto& row : board) {
            for (bool cell : row) {
                result += cell ? '1' : '0';
                result += ' ';
            }
            result += '\n';
        }
        return result;
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
                if (i == 0 && j == 0) continue;
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

    // Setup TCP server
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(12345);
    bind(server, (sockaddr*)&addr, sizeof(addr));
    listen(server, 1);

    std::cout << "[SERVER] Waiting for client on port 12345...\n";
    SOCKET client = accept(server, nullptr, nullptr);
    std::cout << "[SERVER] Client connected!\n";

    while (true) {
        std::string data = game.boardToString();
        int sent = send(client, data.c_str(), data.size(), 0);

        if (sent == SOCKET_ERROR) {
            std::cerr << "[SERVER] Send failed: " << WSAGetLastError() << "\n";
            break;
        }

        std::cout << "[SERVER] Sent board (" << data.size() << " bytes)\n";

        game.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    closesocket(client);
    WSACleanup();
    return 0;
}
