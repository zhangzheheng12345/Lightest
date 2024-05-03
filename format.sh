# clang-format required

# Run this script from root directory of Lightest

echo '** Start formatting **'

clang-format -i include/lightest/*.h
clang-format -i test/*.cpp

echo '** Done formatting **'