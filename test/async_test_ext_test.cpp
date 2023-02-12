#include <lightest/arg_config_ext.h>
#include <lightest/async_test_ext.h>
#include <lightest/data_analysis_ext.h>
#include <lightest/lightest.h>

#undef TEST_FILE_NAME
#define TEST_FILE_NAME "async_test_ext_test.cpp"

ARG_CONFIG();

CONFIG(UseAsyncTest) {
  SET_THREAD_NUM(5);
  USE_ASYNC_GLOBAL();
  USE_ASYNC_SUB();
}

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
  SUB(Sub2) { REQ(1, ==, 2); };
  SUB(TestSubSub) {
    SUB(SubSub1) { REQ(1, ==, 1); };
    SUB(SubSub2) { REQ(1, ==, 2); };
  };
}

REPORT() {
  REPORT_FAILED_TESTS();
  REPORT_PASS_RATE();
  REPORT_AVG_TIME();
}
