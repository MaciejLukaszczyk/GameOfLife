cmake_minimum_required(VERSION 3.30)
project(GameOfLife)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(GameOfLife
    Game_Of_Life_v1.cpp)

target_link_libraries(GameOfLife ws2_32)

