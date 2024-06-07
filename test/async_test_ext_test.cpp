#include <lightest/arg_config_ext.h>
#include <lightest/async_test_ext.h>
#include <lightest/data_analysis_ext.h>
#include <lightest/lightest.h>

#undef TEST_FILE_NAME
#define TEST_FILE_NAME "async_test_ext_test.cpp"

ARG_CONFIG();

CONFIG(UseAsyncTest) { USE_ASYNC(); }

TEST(Test1) {
  REQ(1, ==, 1);
  REQ(1, ==, 2);
}
TEST(Test2) {
  int a = 1, b = 1, c = 2;
  REQ(a, ==, b);
  REQ(a, ==, c);
}
TEST(TestSub) {
  SUB(Sub1) { REQ(1, ==, 1); };
  REQ(1, ==, 2);
  SUB(Sub2) { REQ(1, ==, 2); };
  REQ(1, ==, 2);
  SUB(TestSubSub) {
    SUB(SubSub1) { REQ(1, ==, 1); };
    REQ(1, ==, 2);
    SUB(SubSub2) { REQ(1, ==, 2); };
  };
  REQ(1, ==, 2);
}
TEST(TestCatchingUncaughtError) {
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
TEST(TestAwaiting) {
  REQ(1, ==, 1);
  // TODO: why the total time (reported at the end of the program) << 150ms ? (around 10ms for
  // instace)
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  REQ(1, ==, 2);
  std::cout << "awaited yes!" << std::endl;
}

REPORT() {
  REPORT_FAILED_TESTS();
  REPORT_PASS_RATE();
  REPORT_AVG_TIME();
}
