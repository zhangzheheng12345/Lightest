#include <iostream>
#include "lightest.h"

int main() {
    DEFTEST(hello) {
        MSG("msg")
        WARN("warn")
        ERR("err")
        FAIL("fail")
        REQUIRE(1<=2)
    };
    RUNTEST(hello);
    return 0;
}