#include <iostream>
#include "lightest.h"

int main() {
    DEFTEST(hello) {
        MSG("msg");
        WARN("warn");
        ERR("err");
        FAIL("fail");
        CHECK(1<=2);
        CHECK(3>4);
        REQUIRE(1<=2);
        REQUIRE(3>4);
    };
    RUNTEST(hello);
    return 0;
}