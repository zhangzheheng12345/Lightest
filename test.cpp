#include <iostream>
#include "lightest.h"

int main() {
    DEFTEST(hello) {
        MSG("msg")
        WARN("warn")
        ERR("err")
        FAIL("fail")
        REQUIRE(1<=2)
        REQUIRE(3>4)
    };
    RUNTEST(hello);
    return 0;
}