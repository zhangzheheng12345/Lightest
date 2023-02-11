#include <lightest/arg_config_ext.h>
#include <lightest/data_analysis_ext.h>
#include <lightest/lightest.h>

#undef TEST_FILE_NAME
#define TEST_FILE_NAME "test.cpp"

ARG_CONFIG();

TEST(TestTimerMacros) {
  int i = 0;
  REQ(TIMER(i++), >=, 0);
  REQ(AVG_TIMER(i++, 100), >=, 0);
}

TEST(TestAssertionMacros) {
  REQ(1, ==, 1);
  REQ(1, ==, 2);  // Test fail
  int a = 0, b = 0, c = 1;
  REQ(a, ==, b);
  REQ(b, !=, c);
  REQ(a, >, b);         // Test fail
  MUST(REQ(a, ==, c));  // FAIL & stop this test
}

TEST(TestSub) {
  int a = 1;
  SUB(SubTest1) { REQ(a, ==, 1); };
  SUB(SubTest2) {
    REQ(a, ==, 2);  // Test fail
  };
  SUB(SubTest3) {
    SUB(SubInSubTest) { REQ(a, ==, 1); };
  };
}

TEST(TestCatchUncaughtError) {
  SUB(TestString) { throw "Uncaught string error"; };
  SUB(TestException) {
    class BasicExceptionError : public std::exception {
     public:
      const char* what() const noexcept { return "Uncaught exception error"; }
    };
    throw BasicExceptionError();
  };
  SUB(TestUnknown) { throw 1; };
}

REPORT() {
  REPORT_FAILED_TESTS();
  REPORT_PASS_RATE();
  REPORT_AVG_TIME();
}