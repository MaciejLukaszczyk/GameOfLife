#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

class GameOfLife {
public:
    GameOfLife(size_t rows, size_t cols)
        : rows(rows), cols(cols), board(rows, std::vector<bool>(cols, false)) {}

    void setCell(size_t row, size_t col, bool alive) {
        if (isValidCell(row, col)) {
            board[row][col] = alive;
        }
    }

    void randomize() {
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                board[i][j] = rand() % 2;
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

    void run(size_t iterations, int delay, SOCKET sock) { //zmienić żeby nie otwierało tutaj socketa
        for (size_t i = 0; i < iterations; ++i) {
            sendBoardToServer(sock);
            update();
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

        // Heksagonalne sąsiedztwo (flat-topped layout)
        std::vector<std::pair<int, int>> directions_even = {
            {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, 0}, {1, 1}
        };
        std::vector<std::pair<int, int>> directions_odd = {
            {-1, -1}, {-1, 0}, {0, -1}, {0, 1}, {1, -1}, {1, 0}
        };

        const auto& directions = (row % 2 == 0) ? directions_even : directions_odd;

        for (const auto& [dr, dc] : directions) {
            int nr = static_cast<int>(row) + dr;
            int nc = static_cast<int>(col) + dc;
            if (isValidCell(nr, nc))
                count += board[nr][nc];
        }
        return count;
    }

    void sendBoardToServer(SOCKET sock) const {
        std::string data = std::to_string(rows) + " " + std::to_string(cols) + "\n";
        for (const auto& row : board) {
            for (bool cell : row) {
                data += (cell ? "1" : "0");
                data += " ";
            }
            data += "\n";
        }
        data += "\n"; // kończący separator generacji
        send(sock, data.c_str(), static_cast<int>(data.size()), 0);
    }
};

int main() {
    // Inicjalizacja WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    std::cout << "[CLIENT] Connecting to Python server...\n";
    if (connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[CLIENT] Connection failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "[CLIENT] Connected!\n";

    srand(static_cast<unsigned int>(time(0)));

    size_t rows, cols;
    std::cout << "Enter number of rows: ";
    std::cin >> rows;
    std::cout << "Enter number of columns: ";
    std::cin >> cols;

    GameOfLife game(rows, cols);

    char choice;
    std::cout << "Randomize initial state? (y/n): ";
    std::cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        game.randomize();
    } else {
        std::cout << "Enter initial state (row col alive(1)/dead(0), -1 to end):\n";
        while (true) {
            int r;
            std::cin >> r;
            if (r == -1) break;
            int c, a;
            std::cin >> c >> a;
            game.setCell(static_cast<size_t>(r), static_cast<size_t>(c), a == 1);
        }
    }

    size_t iterations;
    int delay;
    std::cout << "Enter number of iterations: ";
    std::cin >> iterations;
    std::cout << "Enter delay in milliseconds: ";
    std::cin >> delay;

    game.run(iterations, delay, sock);

    closesocket(sock);
    WSACleanup();
    return 0;
}
