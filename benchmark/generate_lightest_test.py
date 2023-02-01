lightestTestFile = open("lightest_test.cpp", "w")

lightestTestFile.write("#include \"lightest/lightest.h\"\n#undef __FILE_NAME__\n#define __FILE_NAME__ \"lightest_test.cpp\"\n\n")

for i in range(1000):
  lightestTestFile.write("TEST(Test" + str(i + 1) + ") { REQ(" + str(i) + ", ==, " + str(i) + "); }\n")

lightestTestFile.close()