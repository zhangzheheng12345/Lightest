#include <lightest/lightest.h>
#include <lightest/data_analysis_ext.h>

TEST(Test1) {
  REQ(1, ==, 1);
}
TEST(Test2) {
  REQ(1, ==, 2);
}
TEST(Test3) {
  SUB(SubTest) {
    SUB(SubSubTest) {
      REQ(1, ==, 1);
    };
  };
}

DATA(IterAll) {
  unsigned int failureCount = 0;
  lightest::IterAll(data, [&failureCount](const lightest::Data* item) {
  if(item->GetFailed()) failureCount++;
    });
  std::cout << "Failures: " << failureCount << std::endl;
}

DATA(IterFailed) {
  unsigned int failureCount = 0;
  lightest::IterFailed(data, [&failureCount](const lightest::Data* item) {
   failureCount++;
    });
  std::cout << "Failures: " << failureCount << std::endl;
}