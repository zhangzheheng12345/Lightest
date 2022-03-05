#include "lightest.h" // Just include lightest.h

int main() {
    DEFCASE(case1) {
        DEFTEST(loggings) {
            MSG("msg");
            WARN("warn");
            ERR("err");
            FAIL("fail");
            int var = 12345;
            LOG(var);
        };
        DEFTEST(assertions) {
            int a = 2;
            REQUIRE(1<=a);
            CHECK(4<a);
            REQ_LOG(a, 4<a); // Same as the first one
            CHK_LOG(a, 4<a); // Same as the second one
        };
    };
    DEFTEST(independent_test) {
        MSG("A message from an independent test!");
        int a = 0;
        CHK_LOG(a, a != 0);
    };
    return 0;
}