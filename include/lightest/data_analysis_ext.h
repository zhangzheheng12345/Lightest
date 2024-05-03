/*
This is a Lightest extension, which provides convenient ways to analyze test
data.
*/

#ifndef _DATA_ANALYSIS_H_
#define _DATA_ANALYSIS_H_

#include "lightest.h"

namespace lightest {

using std::function;

/* ========== Iterating Functions ========== */

// Iterate all the tests (recursively including sub tests)
void IterAllTests(const DataSet* data, function<void(const DataSet*)> func) {
  function<void(const Data*)> iterFunc = [&iterFunc, &func](const Data* item) {
    if (item->Type() == DATA_SET) {
      func(static_cast<const DataSet*>(item));
      static_cast<const DataSet*>(item)->IterSons(iterFunc);
    }
  };
  data->IterSons(iterFunc);
}
// A reload with depth param (start with 0) for the callback func
void IterAllTests(const DataSet* data, function<void(const DataSet*, int)> func) {
  function<void(const Data*, int)> iterFunc = [&iterFunc, &func](const Data* item, int depth) {
    if (item->Type() == DATA_SET) {
      func(static_cast<const DataSet*>(item), depth);
      static_cast<const DataSet*>(item)->IterSons(iterFunc, depth + 1);
    }
  };
  data->IterSons(iterFunc, 0);  // Depth start with 0
}

// Iterate all the failed tests (recursively including sub tests)
void IterFailedTests(const DataSet* data, function<void(const DataSet*)> func) {
  function<void(const Data*)> iterFunc = [&iterFunc, &func](const Data* item) {
    if (item->Type() == DATA_SET && item->GetFailed()) {
      func(static_cast<const DataSet*>(item));
      static_cast<const DataSet*>(item)->IterSons(iterFunc);
    }
  };
  data->IterSons(iterFunc);
}
// A reload with depth param (start with 0) for the callback func
void IterFailedTests(const DataSet* data, function<void(const DataSet*, int)> func) {
  function<void(const Data*, int)> iterFunc = [&iterFunc, &func](const Data* item, int depth) {
    if (item->Type() == DATA_SET && item->GetFailed()) {
      func(static_cast<const DataSet*>(item), depth);
      static_cast<const DataSet*>(item)->IterSons(iterFunc, depth + 1);
    }
  };
  data->IterSons(iterFunc, 0);  // Depth start with 0
}

// A matcher generator
// Each param is a callback func, matching each type extended from Data
function<void(const Data*)> Matcher() {
  return [](const Data* data) { return; };
}
template <typename fstDataType, typename... restDataTypes>
function<void(const Data*)> Matcher(const function<void(const fstDataType*)>& fstFn,
                                    const function<void(const restDataTypes*)>&... restFns) {
  return [=](const Data* data) {
    if (typeid(*data) == typeid(fstDataType))
      return fstFn(static_cast<const fstDataType*>(data));
    else
      Matcher(restFns...)(data);
  };
}

/* ========== Reporting Macros ========== */

// Wrap box for reporting macros
#define REPORT()                                                \
  void ReportWrapFunc(const lightest::DataSet* data);           \
  DATA(Report) {                                                \
    std::cout << "──────────────────────────────" << std::endl  \
              << "# Final report:" << std::endl;                \
    ReportWrapFunc(data);                                       \
    std::cout << "──────────────────────────────" << std::endl; \
  }                                                             \
  void ReportWrapFunc(const lightest::DataSet* data)

// List all the failed tests (recursively including sub tests)
#define REPORT_FAILED_TESTS()                                 \
  do {                                                        \
    std::cout << "Failed tests:" << std::endl;                \
    IterFailedTests(data, [](const lightest::DataSet* item) { \
      item->PrintTabs();                                      \
      std::cout << " * " << item->GetName() << std::endl;     \
    });                                                       \
  } while (0)

// Calculate passing rate and also report the numbers of failed, passed, and
// total tests (just include global tests)
#define REPORT_PASS_RATE()                                                                 \
  do {                                                                                     \
    unsigned int failedTestCount = 0;                                                      \
    data->IterSons([&failedTestCount](const lightest::Data* item) {                        \
      if (item->GetFailed()) failedTestCount++;                                            \
    });                                                                                    \
    std::cout << "Pass rate: " << (1 - double(failedTestCount) / data->GetSonsNum()) * 100 \
              << "% ";                                                                     \
    PRINT_LABEL(lightest::Color::Red, " " << failedTestCount << " failed ");               \
    PRINT_LABEL(lightest::Color::Green,                                                    \
                " " << data->GetSonsNum() - failedTestCount << " passed ");                \
    PRINT_LABEL(lightest::Color::Blue, " " << data->GetSonsNum() << " total ");            \
    lightest::SetColor(lightest::Color::Reset);                                            \
    std::cout << std::endl;                                                                \
  } while (0)

// Calculate & report the average time of global tests
#define REPORT_AVG_TIME()                                                             \
  do {                                                                                \
    clock_t timeSum = 0;                                                              \
    data->IterSons([&timeSum](const lightest::Data* item) {                           \
      timeSum += static_cast<const lightest::DataSet*>(item)->GetDuration();          \
    });                                                                               \
    std::cout << "Average time: " << lightest::TimeToMs(timeSum) / data->GetSonsNum() \
              << " ms" << std::endl;                                                  \
  } while (0)

};  // namespace lightest

#endif