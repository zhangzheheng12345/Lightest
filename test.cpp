#include <iostream>
#include <vector>
#include "lightest.h"

int main() {
    DEFTEST(hello) {
        MSG("msg");
        WARN("warn");
        ERR("err");
        FAIL("fail");
        int a = 2
        if(CHECK(1<=a)) LOG(a);
        if(CHECK(4<a)) LOG(a);
        if(REQUIRE(1<=a)) LOG(a);
        if(REQUIRE(4<a)) LOG(a);
    };
    RUNTEST(hello);
    REPORTTEST();
    return 0;
}