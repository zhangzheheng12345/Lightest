/*
This is a Lightest extension, which provides a async testing system
*/

#ifndef _ASYNC_TEST_H_
#define _ASYNC_TEST_H_

#include <mutex>
#include <thread>
#include "lightest.h"

namespace lightest {

mutex globalDataLock;

class ThreadPool {
 public:
  ThreadPool(unsigned int threadNum_)
      : threadNum(threadNum_),
        tasks(0, TaskWrapper{globalRegisterTest.testData, globalDataLock,
                             [](DataSet* data, mutex& dataLock) {}}) {}
  void SetThreadNum(unsigned int num) { threadNum = num; }
  void AddTask(function<void(DataSet*, mutex&)> func, DataSet* data,
               mutex& dataLock) {
    taskListLock.lock();
    tasks.push_back(TaskWrapper{data, dataLock, func});
    taskListLock.unlock();
  }
  void RunAllTasks() {
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
          TaskWrapper task = tasks.at(index);
          index++;
          taskListLock.unlock();
          task.func(task.data, task.dataLock);
        }
        return;
      });
    for (thread& item : threads) item.join();
  }

 private:
  unsigned int threadNum;
  typedef struct {
    DataSet* data;
    mutex& dataLock;
    function<void(DataSet*, mutex&)> func;
  } TaskWrapper;
  mutex taskListLock;
  vector<TaskWrapper> tasks;
};

ThreadPool globalRegisterAsyncTest(10);
bool useAsyncGlobal = false;  // Use USE_ASYNC_GLOBEL() to set to true
bool useAsyncSub = false;     // Use USE_ASYNC_SUB to set to true

class AddingAsyncTest {
 public:
  AddingAsyncTest(const char* name, function<void(DataSet*, mutex&)> caller,
                  DataSet* data) {
    if (useAsyncGlobal)
      globalRegisterAsyncTest.AddTask(caller, data, globalDataLock);
    else
      globalRegisterTest.Add(name, [caller](Register::Context& ctx) {
        caller(ctx.testData, globalDataLock);
      });
  }
};

void UseAsyncGlobal() {
  useAsyncGlobal = true;
  globalRegisterTest.Add("RunAsyncTests", [](Register::Context& ctx) {
    globalRegisterAsyncTest.RunAllTasks();
  });
}

#define USE_ASYNC_GLOBAL() lightest::UseAsyncGlobal()
#define USE_ASYNC_SUB() lightest::useAsyncSub = true
#define SET_THREAD_NUM(num) lightest::globalRegisterAsyncTest.SetThreadNum(num)

#undef TEST
#define TEST(name)                                                           \
  void name(lightest::Testing& testing, std::mutex& subDataLock);            \
  void call_##name(lightest::DataSet* data, std::mutex& dataLock) {          \
    lightest::Testing testing(#name, 1);                                     \
    static std::mutex subDataLock;                                           \
    const char* errorMsg = CATCH(name(testing, subDataLock));                \
    if (errorMsg) testing.UncaughtError(TEST_FILE_NAME, __LINE__, errorMsg); \
    dataLock.lock();                                                         \
    data->Add(testing.GetData());                                            \
    dataLock.unlock();                                                       \
  }                                                                          \
  lightest::AddingAsyncTest registering_##name(                              \
      #name, call_##name, lightest::globalRegisterTest.testData);            \
  void name(lightest::Testing& testing, std::mutex& subDataLock)

#undef SUB
#define SUB(name)                                                             \
  static std::function<void(lightest::Testing&, std::mutex&)> name;           \
  std::function<void(lightest::DataSet*, std::mutex&)> call_##name =          \
      [&testing](lightest::DataSet* data, std::mutex& dataLock) {             \
        lightest::Testing testing_(#name, testing.GetLevel() + 1);            \
        static std::mutex subDataLock;                                        \
        const char* errorMsg = CATCH(name(testing_, subDataLock));            \
        if (errorMsg)                                                         \
          testing_.UncaughtError(TEST_FILE_NAME, __LINE__, errorMsg);         \
        dataLock.lock();                                                      \
        data->Add(testing_.GetData());                                        \
        dataLock.unlock();                                                    \
      };                                                                      \
  if (lightest::useAsyncSub)                                                  \
                                                                              \
    lightest::globalRegisterAsyncTest.AddTask(call_##name, testing.GetData(), \
                                              subDataLock);                   \
  else                                                                        \
    testing.AddSub(#name, [=](lightest::Register::Context& ctx) {             \
      call_##name(ctx.testData, lightest::globalDataLock);                    \
    });                                                                       \
  name = [=](lightest::Testing & testing, std::mutex & subDataLock)
};  // namespace lightest

#endif