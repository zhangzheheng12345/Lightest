# Generate benchmark source file for Lightest with 1000 tests
lightestTestFile = open("lightest_test.cpp", "w")

lightestTestFile.write("#include \"lightest/lightest.h\"\n#undef __FILE_NAME__\n#define __FILE_NAME__ \"lightest_test.cpp\"\n\n")

# Generate 1000 tests
# e.g. TEST(Test1) { REQ(1, ==, 1); }
for i in range(1000):
  lightestTestFile.write("TEST(Test" + str(i + 1) + ") { REQ(" + str(i) + ", ==, " + str(i) + "); }\n")

lightestTestFile.close()