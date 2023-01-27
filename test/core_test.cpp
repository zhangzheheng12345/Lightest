#include <string>  // To compare string easily in different platforms

#include "lightest/lightest.h"

#undef __FILE_NAME__
#define __FILE_NAME__ "core_test.cpp"

CONFIG(Configurations) {
  for (; argn > 0; argn--, argc++) {
    if (std::string(*argc) == "--no-color") NO_COLOR();
    if (std::string(*argc) == "--no-output") NO_OUTPUT();
  }
  // NO_COLOR();
  // NO_OUTPUT();
}

TEST(TestTimerMacros) {
  int i = 0;
  REQ(TIMER(i++), >=, 0);
  REQ(AVG_TIMER(i++, 100), >=, 0);
}

TEST(TestAssertionMacros) {
  REQ(1, ==, 1);
  REQ(1, ==, 2);  // Test fail
  int a = 0, b = 0, c = 1;
  REQ(a, ==, b);
  REQ(b, !=, c);
  REQ(a, >, b);         // Test fail
  MUST(REQ(a, ==, c));  // FAIL & stop this test
}

TEST(TestSub) {
  int a = 1;
  SUB(SubTest1) { REQ(a, ==, 1); };
  SUB(SubTest2) {
    REQ(a, ==, 2);  // Test fail
  };
  SUB(SubTest3) {
    SUB(SubInSubTest) { REQ(a, ==, 1); };
  };
}

TEST(TestCatchUncaughtError) {
  SUB(TestString) { throw "Uncaught string error"; };
  SUB(TestException) {
    class BasicExceptionError : public std::exception {
     public:
      const char* what() const noexcept { return "Uncaught exception error"; }
    };
    throw BasicExceptionError();
  };
  SUB(TestUnknown) { throw 1; };
}

// To test DATA
DATA(GetFailedTests) {
  unsigned int failedTestCount = 0;
  std::cout << "-----------------------------" << std::endl;
  std::cout << "Failed tests:" << std::endl;
  data->IterSons([&failedTestCount](const lightest::Data* item) {
    const lightest::DataSet* test = static_cast<const lightest::DataSet*>(item);
    if (test->GetFailed()) {
      std::cout << " * " << test->GetName() << std::endl;
      failedTestCount++;
    }
  });
  std::cout << failedTestCount << " test" << (failedTestCount > 1 ? "s" : "")
            << " failed." << std::endl
            << "-----------------------------" << std::endl;
}