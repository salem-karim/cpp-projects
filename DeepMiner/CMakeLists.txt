cmake_minimum_required(VERSION 3.27)
project(DeepMiner)

set(CMAKE_CXX_STANDARD 17)

add_executable(DeepMiner main.cpp src/World.cpp src/Game.cpp src/Robot.cpp
                         src/Player.cpp src/Computer.cpp)
