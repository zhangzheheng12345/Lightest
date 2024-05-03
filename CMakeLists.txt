cmake_minimum_required(VERSION 3.10)

project(Lightest)

set(CMAKE_CXX_STANDARD 11) # C++11 required

message("** Building Lightest Tests **")

# Build Lightest to a header-only library

add_library(${PROJECT_NAME} INTERFACE)
add_library(lightest::lightest ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR}/include)

if(test STREQUAL on)
  message("** Building basic tests **")
  add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()

# Benchmark test (No CTest)
if(benchmark STREQUAL on)
  message("** Building benchmark tests **")
  add_subdirectory(${PROJECT_SOURCE_DIR}/benchmark)
endif()