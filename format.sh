# clang-format required
# in root directory of Lightest

echo 'start formatting'

clang-format -i include/lightest/*.h
clang-format -i test/*.cpp

echo 'done formatting'