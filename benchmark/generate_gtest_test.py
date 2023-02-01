lightestTestFile = open("gtest_test.cpp", "w")

lightestTestFile.write("#include \"gtest/gtest.h\"\n\n")

for i in range(1000):
  lightestTestFile.write("TEST(Test, Test" + str(i + 1) + ") { ASSERT_EQ(" + str(i) + ", " + str(i) + "); }\n")

lightestTestFile.write("int main(int argc, char* argv[]) { testing::InitGoogleTest(&argc, argv); return RUN_ALL_TESTS(); }")

lightestTestFile.close()