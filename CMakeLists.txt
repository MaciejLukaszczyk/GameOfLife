cmake_minimum_required(VERSION 3.30.5)
project(Game_Of_Life_v1.cpp)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(Game_of_Life_250510
        Game_Of_Life_v1.cpp)

target_link_libraries(Game_of_Life_250510 ws2_32)
