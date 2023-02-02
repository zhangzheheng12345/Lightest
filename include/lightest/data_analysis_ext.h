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

#define REPORT()                                                \
  void ReportWrapFunc(const lightest::DataSet* data);           \
  DATA(Report) {                                                \
    std::cout << "------------------------------" << std::endl  \
              << "# Final report:" << std::endl;                \
    ReportWrapFunc(data);                                       \
    std::cout << "------------------------------" << std::endl; \
  }                                                             \
  void ReportWrapFunc(const lightest::DataSet* data)

#define REPORT_FAILED_TESTS()                                 \
  do {                                                        \
    std::cout << "Failed tests:" << std::endl;                \
    IterFailedTests(data, [](const lightest::DataSet* item) { \
      item->PrintTabs();                                      \
      std::cout << " * " << item->GetName() << std::endl;     \
    });                                                       \
  } while (0)

#define REPORT_PASS_RATE()                                                  \
  do {                                                                      \
    unsigned int failedTestCount = 0;                                       \
    data->IterSons([&failedTestCount](const lightest::Data* item) {         \
      if (item->GetFailed()) failedTestCount++;                             \
    });                                                                     \
    std::cout << "Pass rate: "                                              \
              << (1 - double(failedTestCount) / data->GetSonsNum()) * 100   \
              << "% ";                                                      \
    lightest::SetColor(lightest::Color::Red);                               \
    std::cout << " " << failedTestCount << " failed ";                      \
    lightest::SetColor(lightest::Color::Reset);                             \
    lightest::SetColor(lightest::Color::Green);                             \
    std::cout << " " << data->GetSonsNum() - failedTestCount << " passed "; \
    lightest::SetColor(lightest::Color::Blue);                              \
    std::cout << " " << data->GetSonsNum() << " total ";                    \
    lightest::SetColor(lightest::Color::Reset);                             \
    std::cout << std::endl;                                                 \
  } while (0)

};  // namespace lightest

#endif