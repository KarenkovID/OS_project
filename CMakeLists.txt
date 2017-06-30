cmake_minimum_required(VERSION 3.7)
project(OS_project)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
add_executable(OS_project ${SOURCE_FILES})