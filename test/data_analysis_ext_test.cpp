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
TEST(Test4) { throw "Uncaught error"; }

// Test IterAllTests
DATA(TestIterAllTests) {
  unsigned int failureCount = 0;
  lightest::IterAllTests(data, [&failureCount](const lightest::DataSet* item) {
    if (item->GetFailed()) failureCount++;
  });
  std::cout << "** Test IterAllTests: Failures: " << failureCount << std::endl;
}

// Test IterFailedTests
DATA(TestIterFailedTests) {
  unsigned int failureCount = 0;
  lightest::IterFailedTests(
      data, [&failureCount](const lightest::DataSet* item) { failureCount++; });
  std::cout << "** Test IterFailedTests: Failures: " << failureCount << std::endl;
}

DATA(TestMatcher) {
  unsigned int failedReqCount = 0;
  unsigned int uncaughtErrCount = 0;
  std::function<void(const lightest::DataReq*)> matchReq =
      [&failedReqCount](const lightest::DataReq* item) {
        if (item->GetFailed()) failedReqCount++;
      };
  std::function<void(const lightest::DataUncaughtError*)> matchErr =
      [&uncaughtErrCount](const lightest::DataUncaughtError* item) { uncaughtErrCount++; };
  std::function<void(const lightest::DataSet*)> matchDataSet =
      [&matchReq, &matchErr, &matchDataSet](const lightest::DataSet* item) {
        item->IterSons(lightest::Matcher(matchReq, matchErr, matchDataSet));
      };
  data->IterSons(lightest::Matcher(matchReq, matchErr, matchDataSet));
  std::cout << "** Test Matcher: FailedReqCount: " << failedReqCount << std::endl
            << "** Test Matcher: UncaughtErrCount: " << uncaughtErrCount << std::endl;
}

// Test REPORT
REPORT() {
  REPORT_FAILED_TESTS();
  REPORT_PASS_RATE();
  REPORT_AVG_TIME();
}