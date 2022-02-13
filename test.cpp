#include <iostream> // You MUST include iostream!
#include "lightest.h"

int main() {
    DEFCASE(case1) {
        DEFTEST(loggings) {
            MSG("msg");
            WARN("warn");
            ERR("err");
            FAIL("fail");
        };
        RUNTEST(loggings);
        DEFTEST(assertions) {
            int a = 2;
            if(CHECK(1<=a)) LOG(a);
            if(CHECK(4<a)) LOG(a);
            if(REQUIRE(1<=a)) LOG(a);
            if(REQUIRE(4<a)) LOG(a);
            REQ_LOG(a, 4<a); // Same as the first one
            CHK_LOG(a, 4<a); // Same as the second one
        };
        RUNTEST(assertions);
    };
    RUNCASE(case1);
    REPORTTOTAL();
    return 0;
}