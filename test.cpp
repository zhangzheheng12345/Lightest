#include "lightest.h" // Just include lightest.h

int main() {
    DEFCASE(case1) {
        DEFTEST(loggings) {
            MSG("msg");
            WARN("warn");
            ERR("err");
            FAIL("fail");
        };
        DEFTEST(assertions) {
            int a = 2;
            REQUIRE(1<=a);
            CHECK(4<a);
            REQ_LOG(a, 4<a); // Same as the first one
            CHK_LOG(a, 4<a); // Same as the second one
        };
    };
    return 0;
}