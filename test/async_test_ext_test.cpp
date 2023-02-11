#include <lightest/async_test_ext.h>
#include <lightest/lightest.h>

CONFIG(UseAsyncTest) { USE_ASYNC_GLOBAL(); }

TEST(TestThreadPool) { REQ(1, ==, 2); }