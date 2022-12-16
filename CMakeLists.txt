cmake_minimum_required(VERSION 3.10)

project(Lightest)

set(build_example false)
# To build example
# set(build_example true)

set(CMAKE_CXX_STANDARD 11)

if(${build_example})
    add_executable(Lightest ./src/example.cpp)
else()
    add_executable(Lightest ./src/test.cpp)
endif()