cmake_minimum_required(VERSION 3.10)

project(Lightest)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SRC_FILES
    "${PROJECT_SOURCE_DIR}/src/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})