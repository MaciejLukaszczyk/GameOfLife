cmake_minimum_required(VERSION 3.30.5)
project(GameOfLife)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(GameOfLife
    Game_Of_Life_v1.cpp)
