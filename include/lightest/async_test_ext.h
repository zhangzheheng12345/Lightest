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
      : threadNum(threadNum_), tasks(0, [](DataSet* data) {}) {}
  void AddTask(function<void(DataSet*)> func) { tasks.push_back(func); }
  void RunAllTasks(DataSet* data) {
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
          function<void(DataSet*)> func = tasks.at(index);
          index++;
          taskListLock.unlock();
          func(data);
        }
        return;
      });
    for (thread& item : threads) item.join();
  }

 private:
  unsigned int threadNum;
  mutex taskListLock;
  vector<function<void(DataSet*)>> tasks;
};

};  // namespace lightest

#endif