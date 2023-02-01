cmake_minimum_required(VERSION 3.10)

project(LightestBenchmark)

# To build benchmark:
# cmake -Dbenchmark=on

# Compared with GTest, so GTest should be pre-installed
find_package(GTest REQUIRED)

# Generate benchmark source files via Python:
# python generate_lightest_test.py
# python generate_gtest_test.py

add_executable(LightestBenchmarkLightest lightest_test.cpp)
target_link_libraries(LightestBenchmarkLightest lightest::lightest)

include_directories(${GTEST_INCLUDE})
add_executable(LightestBenchmarkGTest gtest_test.cpp)
target_link_libraries(LightestBenchmarkGTest ${GTEST_BOTH_LIBRARIES} pthread)