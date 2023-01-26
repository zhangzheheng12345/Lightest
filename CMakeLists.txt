cmake_minimum_required(VERSION 3.10)

project(Lightest)

set(CMAKE_CXX_STANDARD 11) # C++11 required

# Build Lightest to a header-only library

add_library(${PROJECT_NAME} INTERFACE)
add_library(lightest::lightest ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR}/include)

# Do not use CTest, for testing outputs should be given out by Lightest

add_subdirectory(${PROJECT_SOURCE_DIR}/test)