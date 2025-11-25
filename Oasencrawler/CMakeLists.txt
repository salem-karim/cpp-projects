cmake_minimum_required(VERSION 3.27)
project(Oasencrawler)

set(CMAKE_CXX_STANDARD 14)

add_executable(Oasencrawler main.cpp Player/Player.cpp World/World.cpp
                            Enemy/Enemy.cpp)
