#include <lightest/arg_config_ext.h>
#include <lightest/data_analysis_ext.h>
#include <lightest/lightest.h>

#undef TEST_FILE_NAME
#define TEST_FILE_NAME "data_analysis_ext_test.cpp"

ARG_CONFIG();

// Small tests to provide test data
TEST(Test1) { REQ(1, ==, 1); }
TEST(Test2) { REQ(1, ==, 2); }
TEST(Test3) {
  SUB(SubTest) {
    SUB(SubSubTest) { REQ(1, ==, 2); };
  };
}

// Test IterAllTests
DATA(IterAllTests) {
  unsigned int failureCount = 0;
  lightest::IterAllTests(data, [&failureCount](const lightest::DataSet* item) {
    if (item->GetFailed()) failureCount++;
  });
  std::cout << "Test IterAllTests: Failures: " << failureCount << std::endl;
}

// Test IterFailedTests
DATA(IterFailedTests) {
  unsigned int failureCount = 0;
  lightest::IterFailedTests(
      data, [&failureCount](const lightest::DataSet* item) { failureCount++; });
  std::cout << "Test IterFailedTests: Failures: " << failureCount << std::endl;
}

// Test REPORT
REPORT() {
  REPORT_FAILED_TESTS();
  REPORT_PASS_RATE();
  REPORT_AVG_TIME();
}