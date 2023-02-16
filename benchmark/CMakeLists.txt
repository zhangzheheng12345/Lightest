cmake_minimum_required(VERSION 3.10)

project(LightestBenchmark)

# To build benchmark:
# cmake -Dbenchmark=on

# Call Python generators for Lightest & GTest
# ./generate_tests.sh

add_executable(LightestBenchmarkLightest lightest_test.cpp)
target_link_libraries(LightestBenchmarkLightest lightest::lightest)

add_executable(LightestBenchmarkLightestAsync lightest_async_test.cpp)
target_link_libraries(LightestBenchmarkLightestAsync lightest::lightest)

# Sometimes need to ignore GTest build
if(NOT gtest STREQUAL off)
  # Compared with GTest, so GTest should be pre-installed
  find_package(GTest REQUIRED)
  include_directories(${GTEST_INCLUDE})
  add_executable(LightestBenchmarkGTest gtest_test.cpp)
  target_link_libraries(LightestBenchmarkGTest ${GTEST_BOTH_LIBRARIES} pthread)
endif()