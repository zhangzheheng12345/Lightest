#include <lightest/async_test_ext.h>
#include <lightest/lightest.h>

TEST(TestThreadPool) {
  lightest::ThreadPool threadPool(10);
  for (unsigned int i = 0; i < 100; i++) {
    threadPool.AddTask(
        [=](lightest::DataSet* data) { std::cout << i << std::endl; });
  }
  threadPool.RunAllTasks(lightest::globalRegisterTest.testData);
}