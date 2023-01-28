cmake_minimum_required(VERSION 3.10)

project(LightestTest)

add_executable(${PROJECT_NAME} core_test.cpp)
target_link_libraries(${PROJECT_NAME} lightest::lightest)

# Do not use CTest, for testing outputs should be given out by Lightest