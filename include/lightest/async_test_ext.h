/*
This is a Lightest extension, which provides a async testing system
*/

#ifndef _ASYNC_TEST_H_
#define _ASYNC_TEST_H_

#include <mutex>
#include <thread>
#include "lightest.h"

namespace lightest {

/* ========== Engine for Async Testing ========== */

mutex globalDataLock;

// A simple thread pool to run any tests
class ThreadPool {
 public:
  ThreadPool(unsigned int threadNum_)
      : threadNum(threadNum_),
        tasks(0, TaskWrapper{globalRegisterTest.testData, globalDataLock,
                             [](DataSet* data, mutex& dataLock) {}}) {}
  void SetThreadNum(unsigned int num) { threadNum = num; }
  void AddTask(function<void(DataSet*, mutex&)> func, DataSet* data,
               mutex& dataLock) {
    taskListLock.lock();  // Safely add new tasks while running
    tasks.push_back(TaskWrapper{data, dataLock, func});
    taskListLock.unlock();
  }
  // Keep running tasks until the task list is empty
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
  mutex taskListLock;
  unsigned int threadNum;
  typedef struct {
    DataSet* data;    // Parent data object
    mutex& dataLock;  // The lock for the data object above
    function<void(DataSet*, mutex&)> func;
  } TaskWrapper;
  vector<TaskWrapper> tasks;
};

ThreadPool asyncTestRunner(10);  // Default 10 threads
bool useAsyncGlobal = false;     // Use USE_ASYNC_GLOBEL() to set to true
bool useAsyncSub = false;        // Use USE_ASYNC_SUB to set to true

class AddingAsyncTest {
 public:
  AddingAsyncTest(const char* name, function<void(DataSet*, mutex&)> caller,
                  DataSet* data) {
    if (useAsyncGlobal)
      asyncTestRunner.AddTask(caller, data, globalDataLock);
    else
      globalRegisterTest.Add(name, [caller](Register::Context& ctx) {
        caller(ctx.testData, globalDataLock);
      });
  }
};

/* ========== Configuartions for Async Testing ========== */

void UseAsyncGlobal() {
  useAsyncGlobal = true;
  // Inject a 'global test' which launches the async testing engine at the time
  // when sync testing is started
  globalRegisterTest.Add("RunAsyncTests", [](Register::Context& ctx) {
    asyncTestRunner.RunAllTasks();
  });
}

// Enable async testing only for  global tests
#define USE_ASYNC_GLOBAL() lightest::UseAsyncGlobal()

// Enable async testing for sub tests
#define USE_ASYNC_SUB() lightest::useAsyncSub = true

// Change the number of threads
#define SET_THREAD_NUM(num) lightest::asyncTestRunner.SetThreadNum(num)

/* =========== Redefined Macros for Async Testing ========== */

#undef TEST
#define TEST(name)                                                      \
  void name(lightest::Testing& testing, std::mutex& subDataLock);       \
  void call_##name(lightest::DataSet* data, std::mutex& dataLock) {     \
    static lightest::Testing testing(#name, 1);                         \
    static std::mutex subDataLock; /* The lock for current test data */ \
    const char* errorMsg = CATCH(name(testing, subDataLock));           \
    if (errorMsg) {                                                     \
      subDataLock.lock(); /* Safely add uncaught error data */          \
      testing.UncaughtError(TEST_FILE_NAME, __LINE__, errorMsg);        \
      subDataLock.unlock();                                             \
    }                                                                   \
    dataLock.lock(); /* Safely add data to parent */                    \
    data->Add(testing.GetData());                                       \
    dataLock.unlock();                                                  \
  }                                                                     \
  lightest::AddingAsyncTest registering_##name(                         \
      #name, call_##name, lightest::globalRegisterTest.testData);       \
  void name(lightest::Testing& testing, std::mutex& subDataLock)

#undef SUB
#define SUB(name)                                                           \
  static std::function<void(lightest::Testing&, std::mutex&)> name;         \
  std::function<void(lightest::DataSet*, std::mutex&)> call_##name =        \
      [&testing](lightest::DataSet* data, std::mutex& dataLock) {           \
        static lightest::Testing testing_(#name, testing.GetLevel() + 1);   \
        static std::mutex subDataLock; /* The lock for current test data */ \
        const char* errorMsg = CATCH(name(testing_, subDataLock));          \
        if (errorMsg) {                                                     \
          subDataLock.lock(); /* Safely add uncaught error data */          \
          testing_.UncaughtError(TEST_FILE_NAME, __LINE__, errorMsg);       \
          subDataLock.unlock();                                             \
        }                                                                   \
        dataLock.lock(); /* Safely add data to parent */                    \
        data->Add(testing_.GetData());                                      \
        dataLock.unlock();                                                  \
      };                                                                    \
  if (lightest::useAsyncSub) /* Async sub testing */                        \
    lightest::asyncTestRunner.AddTask(call_##name, testing.GetData(),       \
                                      subDataLock);                         \
  else /* Sync sub testing */                                               \
    testing.AddSub(#name,                                                   \
                   [=, &subDataLock](lightest::Register::Context& ctx) {    \
                     call_##name(ctx.testData, subDataLock);                \
                   }); /* A lambda wrapping to fit input */                 \
  name = [=](lightest::Testing & testing, std::mutex & subDataLock)

#undef REQ
#define REQ(actual, operator, expected)                                \
  ([&]() -> bool {                                                     \
    bool res = (actual) operator(expected);                            \
    subDataLock.lock(); /* Safely add assertion data to parent */      \
    testing.Req(TEST_FILE_NAME, __LINE__, actual, expected, #operator, \
                #actual " " #operator" " #expected, !res);             \
    subDataLock.unlock();                                              \
    return res;                                                        \
  })()

};  // namespace lightest

#endif