/*
This is a Lightest extension, which provides convenient ways to analyze test
data.
*/

#ifndef _DATA_ANALYSIS_H_
#define _DATA_ANALYSIS_H_

#include "lightest.h"

namespace lightest {

/* ========== Iterating Functions ========== */

void IterAllTests(const DataSet* data, function<void(const DataSet*)> func) {
  std::function<void(const lightest::Data*)> iterFunc =
      [&iterFunc, &func](const lightest::Data* item) {
        if (item->Type() == lightest::DATA_SET) {
          func(static_cast<const DataSet*>(item));
          static_cast<const lightest::DataSet*>(item)->IterSons(iterFunc);
        }
      };
  data->IterSons(iterFunc);
}

void IterFailedTests(const DataSet* data, function<void(const DataSet*)> func) {
  std::function<void(const lightest::Data*)> iterFunc =
      [&iterFunc, &func](const lightest::Data* item) {
        if (item->Type() == lightest::DATA_SET && item->GetFailed()) {
          func(static_cast<const DataSet*>(item));
          static_cast<const lightest::DataSet*>(item)->IterSons(iterFunc);
        }
      };
  data->IterSons(iterFunc);
}

/* ========== Reporting Functions ========== */

#define REPORT_FAILED_TESTS()                                                 \
  DATA(ReportFailedTests) {                                                   \
    std::cout << "------------------------------" << std::endl                \
              << "Failed tests:" << std::endl;                                \
    unsigned int failedTestCount = 0;                                         \
    IterFailedTests(data, [&failedTestCount](const lightest::DataSet* item) { \
      item->PrintTabs();                                                      \
      std::cout << " * " << item->GetName() << std::endl;                     \
      if (item->GetTabs() == 1)                                               \
        failedTestCount++; /* Only count global failed tests */               \
    });                                                                       \
    std::cout << failedTestCount << " test"                                   \
              << (failedTestCount > 1 ? "s" : "") << " failed." << std::endl  \
              << "------------------------------" << std::endl;               \
  }

#define REPORT_PASS_RATE()                                                \
  DATA(ReportPassRate) {                                                  \
    unsigned int failedTestCount = 0;                                     \
    data->IterSons([&failedTestCount](const lightest::Data* item) {       \
      if (item->GetFailed()) failedTestCount++;                           \
    });                                                                   \
    std::cout << "Pass rate: "                                            \
              << (1 - double(failedTestCount) / data->GetSonsNum()) * 100 \
              << "%" << std::endl;                                        \
  }

};  // namespace lightest

#endif