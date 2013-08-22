cmake_minimum_required(VERSION 2.6)

project(openage)

#main build configuration file

#eventually use clang
#set(CMAKE_CXX_COMPILER "clang++" )

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++11")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib")


set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Werror")

add_subdirectory("src")
