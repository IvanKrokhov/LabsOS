cmake_minimum_required(VERSION 3.30)
project(windows)

set(CMAKE_CXX_STANDARD 20)

add_executable(windows main.cpp
        killer.cpp
        user.cpp)
