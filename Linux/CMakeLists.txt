cmake_minimum_required(VERSION 3.30)
project(Linux)

set(CMAKE_CXX_STANDARD 20)

add_executable(Linux main.cpp
        Number.h
        Number.cpp
        Vector.h
        Vector.cpp)
