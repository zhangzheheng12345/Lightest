#include <iostream>
#include <vector>
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
        };
        RUNTEST(assertions);
        
    };
    RUNCASE(case1);
    REPORTTOTAL();
    return 0;
}