cmake_minimum_required(VERSION 3.10)

project(LightestTest)

add_executable(LightestCoreTest core_test.cpp)
target_link_libraries(LightestCoreTest lightest::lightest)

add_executable(LightestDataAnalysisExtTest data_analysis_ext_test.cpp)
target_link_libraries(LightestDataAnalysisExtTest lightest::lightest)

# Do not use CTest, for testing outputs should be given out by Lightest