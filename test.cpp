#include <iostream>
#include <vector>
#include "lightest.h"

int main() {
    DEFCASE(case1) {
        DEFTEST(hello) {
            MSG("msg");
            WARN("warn");
            ERR("err");
            FAIL("fail");
            int a = 2;
            if(CHECK(1<=a)) LOG(a);
            if(CHECK(4<a)) LOG(a);
            if(REQUIRE(1<=a)) LOG(a);
            if(REQUIRE(4<a)) LOG(a);
        };
        RUNTEST(hello);
    };
    RUNCASE(case1);
    REPORTTEST();
    return 0;
}