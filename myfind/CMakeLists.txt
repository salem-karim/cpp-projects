cmake_minimum_required(VERSION 3.29)
project(myfind)

set(CMAKE_CXX_STANDARD 26)

add_executable(myfind main.cpp
        search.cpp
        fork_test.cpp
        search.h)
