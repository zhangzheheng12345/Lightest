cmake_minimum_required(VERSION 3.10)

project(LightestTest)

file(GLOB TEST_FILES
    "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(${PROJECT_NAME} ${TEST_FILES})
target_link_libraries(${PROJECT_NAME} lightest::lightest)

# Do not use CTest, for testing outputs should be given out by Lightest