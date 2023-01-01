#include <string>  // To compare string easily in different platforms

#include "lightest/lightest.h"
// #define __FILE__ "test.cpp"

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
  REQ(a, >, b);  // Test fail
  REQ(a, ==, b);
  REQ(b, !=, c);
  MUST(REQ(a, ==, c));  // FAIL & stop this test
}

TEST(TestSub) {
  int a = 1;
  SUB(SubTest1) { REQ(a, ==, 1); };
  SUB(SubTest2) {
    REQ(a, ==, 2);  // Test fail
  };
}

// To test DATA
DATA(GetFailedTests) {
  std::cout << "-----------------------------" << std::endl;
  std::cout << "Failed tests:" << std::endl;
  data->IterSons([](const lightest::Data* item) {
    const lightest::DataSet* test = static_cast<const lightest::DataSet*>(item);
    if (test->GetFailed()) {
      std::cout << " * " << test->name << std::endl;
    }
  });
  std::cout << "-----------------------------" << std::endl;
}