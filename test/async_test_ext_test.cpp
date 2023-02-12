#include <lightest/async_test_ext.h>
#include <lightest/data_analysis_ext.h>
#include <lightest/lightest.h>

#undef TEST_FILE_NAME
#define TEST_FILE_NAME "async_test_ext_test.cpp"

CONFIG(UseAsyncTest) {
  SET_THREAD_NUM(5);
  USE_ASYNC_GLOBAL();
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

REPORT() {
  REPORT_FAILED_TESTS();
  REPORT_PASS_RATE();
  REPORT_AVG_TIME();
}
