cmake_minimum_required(VERSION 3.30)
project(GameOfLife)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(GameOfLife
        Application.cpp
        include/Application.h
        GameOfLife.cpp
        include/GameOfLife.h
        NetworkManager.cpp
        include/NetworkManager.h
        main.cpp)

target_link_libraries(GameOfLife ws2_32)

