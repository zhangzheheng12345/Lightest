/*
This is a Lightest extension, which provides a async testing system
*/

#ifndef _ASYNC_TEST_H_
#define _ASYNC_TEST_H_

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include "lightest.h"

namespace lightest {

/* ========== Engine for Async Testing ========== */

mutex globalDataLock;

// To ensure all tests to be run
class TestTree {
 public:
  TestTree(TestTree* parent_) {
    parent = parent_;
    if (parent) parent->AddUndoneSon();
    thisDone = false;
    undoneSonNum = 0;
  }
  bool GetDone() const { return thisDone && !(undoneSonNum.load()); }
  void AddUndoneSon() { undoneSonNum.fetch_add(1); }
  void AddDoneSon() { undoneSonNum.fetch_add(-1); }
  void SetThisDone() {
    thisDone = true;
    if (parent) parent->AddDoneSon();
  }

 private:
  atomic<bool> thisDone;
  atomic<unsigned int> undoneSonNum;
  TestTree* parent;
};

// A simple thread pool to run any tests
class ThreadPool {
 public:
  ThreadPool(unsigned int threadNum_) {
    threadNum = threadNum_;
    tasks = vector<TaskWrapper>();
    testTreeRoot = new TestTree(nullptr);
    testTreeRoot->SetThisDone();
  }
  void SetThreadNum(unsigned int num) { threadNum = num; }
  void AddTask(function<void(DataSet*, mutex&, TestTree*)> func, DataSet* data,
               mutex& dataLock, TestTree* node) {
    taskListLock.lock();  // Safely add new tasks while running
    tasks.push_back(TaskWrapper{data, dataLock, func, node});
    taskListLock.unlock();
  }
  // Keep running tasks until the task list is empty
  void RunAllTasks() {
    unsigned int index = 0;
    vector<thread> threads(threadNum);
    for (thread& item : threads)
      item = thread([&]() {
        while (true) {
          taskListLock.lock();
          if (tasks.size() <= index) {
            taskListLock.unlock();
            return;
          }
          TaskWrapper task = tasks.at(index);
          index++;
          taskListLock.unlock();
          task.func(task.data, task.dataLock, task.node);
        }
        return;
      });
    unsigned int baseNum = tasks.size();
    for (thread& item : threads) item.detach();
    // Check done before return
    while (!testTreeRoot->GetDone()) {
      const unsigned int checkGapTime = 50;
      this_thread::sleep_for(chrono::milliseconds(checkGapTime));
    }
  }
  TestTree* GetTestTreeRoot() const { return testTreeRoot; }  // No const -- must be changable

 private:
  mutex taskListLock;
  unsigned int threadNum;
  typedef struct {
    DataSet* data;    // Parent data object
    mutex& dataLock;  // The lock for the data object above
    function<void(DataSet*, mutex&, TestTree*)> func;
    TestTree* node;  // for THIS test instead of the parent
  } TaskWrapper;
  vector<TaskWrapper> tasks;
  TestTree* testTreeRoot;
};

const unsigned int defaultThreadNum = 10;
ThreadPool asyncTestRunner(defaultThreadNum);

class AddingAsyncTest {
 public:
  AddingAsyncTest(const char* name, function<void(DataSet*, mutex&, TestTree*)> caller,
                  DataSet* data) {
    asyncTestRunner.AddTask(caller, data, globalDataLock,
                            new TestTree(asyncTestRunner.GetTestTreeRoot()));
  }
};

/* ========== Configuartions for Async Testing ========== */

void UseAsync() {
  // Inject a 'global test' which launches the async testing engine at the time
  // when sync testing is started
  globalRegisterTest.Add("RunAsyncTests",
                         [](Register::Context& ctx) { asyncTestRunner.RunAllTasks(); });
}

// Change the number of threads
#define SET_THREAD_NUM(num) lightest::asyncTestRunner.SetThreadNum(num)
#define USE_ASYNC() lightest::UseAsync()

/* =========== Redefined Macros for Async Testing ========== */

#undef TEST
#define TEST(name)                                                                            \
  void name(lightest::Testing& testing, std::mutex& subDataLock, lightest::TestTree* node);   \
  void call_##name(lightest::DataSet* data, std::mutex& dataLock, lightest::TestTree* node) { \
    static lightest::Testing testing(#name, 1);                                               \
    static std::mutex subDataLock; /* The lock for current test data */                       \
    const char* errorMsg = CATCH(name(testing, subDataLock, node));                           \
    node->SetThisDone();                                                                      \
    if (errorMsg) {                                                                           \
      subDataLock.lock(); /* Safely add uncaught error data */                                \
      testing.AddData(new lightest::DataUncaughtError(TEST_FILE_NAME, __LINE__, errorMsg));   \
      subDataLock.unlock();                                                                   \
    }                                                                                         \
    dataLock.lock(); /* Safely add data to parent */                                          \
    data->Add(testing.GetData());                                                             \
    dataLock.unlock();                                                                        \
  }                                                                                           \
  lightest::AddingAsyncTest registering_##name(#name, call_##name,                            \
                                               lightest::globalRegisterTest.testData);        \
  void name(lightest::Testing& testing, std::mutex& subDataLock, lightest::TestTree* node)

#undef SUB
#define SUB(name)                                                                           \
  static std::function<void(lightest::Testing&, std::mutex&, lightest::TestTree*)> name;    \
  std::function<void(lightest::DataSet*, std::mutex&, lightest::TestTree*)> call_##name =   \
      [&testing](lightest::DataSet* data, std::mutex& dataLock, lightest::TestTree* node) { \
        static lightest::Testing testing_(#name, testing.GetLevel() + 1);                   \
        static std::mutex subDataLock; /* The lock for current test data */                 \
        const char* errorMsg = CATCH(name(testing_, subDataLock, node));                    \
        node->SetThisDone();                                                                \
        if (errorMsg) {                                                                     \
          subDataLock.lock(); /* Safely add uncaught error data */                          \
          testing_.AddData(                                                                 \
              new lightest::DataUncaughtError(TEST_FILE_NAME, __LINE__, errorMsg));         \
          subDataLock.unlock();                                                             \
        }                                                                                   \
        dataLock.lock(); /* Safely add data to parent */                                    \
        data->Add(testing_.GetData());                                                      \
        dataLock.unlock();                                                                  \
      };                                                                                    \
  lightest::asyncTestRunner.AddTask(call_##name, testing.GetData(), subDataLock,            \
                                    new lightest::TestTree(node));                          \
  name = [=](lightest::Testing & testing, std::mutex & subDataLock, lightest::TestTree * node)

#undef REQ
#define REQ(actual, operator, expected)                                                     \
  ([&]() -> bool {                                                                          \
    auto actual_ = (actual);                                                                \
    auto expected_ = (expected);                                                            \
    bool res = (actual_) operator(expected_);                                               \
    subDataLock.lock(); /* Safely add assertion data to parent */                           \
    testing.AddData(new lightest::DataReq(                                                  \
        TEST_FILE_NAME, __LINE__, lightest::AnyToString(actual_),                           \
        lightest::AnyToString(expected_), typeid(actual_).name(), typeid(expected_).name(), \
        #operator, #actual " " #operator" " #expected, !res));                              \
    subDataLock.unlock();                                                                   \
    return res;                                                                             \
  })()

};  // namespace lightest

#endif