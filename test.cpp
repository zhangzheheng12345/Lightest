#include <iostream>
#include "lightest.h"

int main() {
    DEFTEST(hello) {
        MSG("msg")
        WARN("warn")
        ERR("err")
        FAIL("fail")
    };
    RUNTEST(hello);
    return 0;
}