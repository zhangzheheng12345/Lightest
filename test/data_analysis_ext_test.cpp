#include <lightest/data_analysis_ext.h>
#include <lightest/lightest.h>

#undef __FILE_NAME__
#define __FILE_NAME__ "data_analysis_ext_test.cpp"

CONFIG(Configuration) {
  RETURN_ZERO();
}

TEST(Test1) { REQ(1, ==, 1); }
TEST(Test2) { REQ(1, ==, 2); }
TEST(Test3) {
  SUB(SubTest) {
    SUB(SubSubTest) { REQ(1, ==, 2); };
  };
}

DATA(IterAllTests) {
  unsigned int failureCount = 0;
  lightest::IterAllTests(data, [&failureCount](const lightest::DataSet* item) {
    if (item->GetFailed()) failureCount++;
  });
  std::cout << "Failures: " << failureCount << std::endl;
}

DATA(IterFailedTests) {
  unsigned int failureCount = 0;
  lightest::IterFailedTests(
      data, [&failureCount](const lightest::DataSet* item) { failureCount++; });
  std::cout << "Failures: " << failureCount << std::endl;
}

REPORT_FAILED_TESTS();
REPORT_PASS_RATE();