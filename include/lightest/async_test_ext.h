/*
This is a Lightest extension, which provides a async testing system
*/

#ifndef _ASYNC_TEST_H_
#define _ASYNC_TEST_H_

#include <mutex>
#include <thread>
#include "lightest.h"

namespace lightest {

class ThreadPool {
 public:
  ThreadPool(unsigned int threadNum_)
      : threadNum(threadNum_),
        tasks(0, [](DataSet* data, mutex& dataLock) {}) {}
  void SetThreadNum(unsigned int num) { threadNum = num; }
  void AddTask(function<void(DataSet*, mutex&)> func) { tasks.push_back(func); }
  void RunAllTasks(DataSet* data) {
    mutex taskListLock, dataLock;
    unsigned int index = 0;
    vector<thread> threads(threadNum);
    for (thread& item : threads)
      item = thread([&]() {
        while (1) {
          taskListLock.lock();
          if (tasks.size() <= index) {
            taskListLock.unlock();
            return;
          }
          function<void(DataSet*, mutex&)> func = tasks.at(index);
          index++;
          taskListLock.unlock();
          func(data, dataLock);
        }
        return;
      });
    for (thread& item : threads) item.join();
  }

 private:
  unsigned int threadNum;
  vector<function<void(DataSet*, mutex&)>> tasks;
};

class AddingAsyncTest {
 public:
  AddingAsyncTest(ThreadPool& pool, function<void(DataSet*, mutex&)> caller) {
    pool.AddTask(caller);
  }
};

ThreadPool globalRegisterAsyncTest(10);
bool useAsyncGlobal = false;  // Use USE_ASYNC_GLOBEL() to set to true

void UseAsyncGlobal() {
  useAsyncGlobal = true;
  globalRegisterTest.Add("RunAsyncTests", [](Register::Context& ctx) {
    globalRegisterAsyncTest.RunAllTasks(ctx.testData);
  });
}

#define USE_ASYNC_GLOBAL() lightest::UseAsyncGlobal()
#define SET_THREAD_NUM(num) lightest::globalRegisterAsyncTest.SetThreadNum(num)

#undef TEST
#define TEST(name)                                                           \
  void name(lightest::Testing& testing);                                     \
  void call_##name(lightest::DataSet* data, std::mutex& dataLock) {          \
    if (!lightest::useAsyncGlobal) return;                                   \
    lightest::Testing testing(#name, 1);                                     \
    const char* errorMsg = CATCH(name(testing));                             \
    if (errorMsg) testing.UncaughtError(TEST_FILE_NAME, __LINE__, errorMsg); \
    dataLock.lock();                                                         \
    data->Add(testing.GetData());                                            \
    dataLock.unlock(); /* Colletct data */                                   \
  }                                                                          \
  lightest::AddingAsyncTest registering_##name(                              \
      lightest::globalRegisterAsyncTest, call_##name);                       \
  void name(lightest::Testing& testing)

};  // namespace lightest

#endif