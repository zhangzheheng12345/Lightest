#include <string>  // To compare string easily in different platforms

#include "lightest/lightest.h"
#ifndef __FILE__  // because warning: "__FILE__" redefined [-Wbuiltin-macro-redefined]
#define __FILE__ "test.cpp"
#endif

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

// some trivial test invocations, can get compiled to create a single FAILED or to create all PASS test results
TEST( Four_times_okay )
{
  REQ( 1, ==, 1 );
  REQ( 1, >, 0 );
  REQ( 1, <, 2 );
  REQ( 1, !=, 2 );
}

#if defined(FAIL) && (FAIL>=1)
TEST( One_okay_one_failed_one_okay )
{
  REQ( 1, ==, 1 );
  REQ( 1, ==, 2 );
  REQ( 2, ==, 2 );
}
#else
TEST( Three_times_okay )
{
  REQ( 1, ==, 1 );
  REQ( 2, ==, 2 );
  REQ( 3, ==, 3 );
}
#endif

TEST( One_okay )
{
  REQ( 5, ==, 5 );
}


TEST(TestAssertionMacros) {
  REQ(1, ==, 1);
# if defined(FAIL) && (FAIL>=2)
  REQ(1, ==, 2);  // Test fail
# endif
  int a = 0, b = 0, c = 1;
  REQ(a, ==, b);
  REQ(b, !=, c);
# if defined(FAIL) && (FAIL>=3)
  REQ(a, >, b);         // Test fail
  MUST(REQ(a, ==, c));  // FAIL & stop this test
# else
  REQ(a, >=, b);        // Test passed
  MUST(REQ(a, <=, c));  // PASS & not stop this test
# endif
}

TEST(TestSub) {
  int a = 1;
  SUB(SubTest1) { REQ(a, ==, 1); };
  SUB(SubTest2) {
#   if defined(FAIL) && (FAIL>=4)
    REQ(a, ==, 2);  // Test fail
#   else
    REQ(a, <=, 2);  // Test pass
#   endif
  };
  SUB(SubTest3) {
    SUB(SubInSubTest) { REQ(a, ==, 1); };
  };
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
  if (lightest::toOutput) {
    std::cout << "Now the overall report:" << std::endl;
  }
  return; // just for easy breakpoint setting while debugging failed tests.
}