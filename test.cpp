#include <iostream>
#include "lightest.h"

int main() {
    DEFTEST(hello) {
        MSG("msg")
        FAIL("fail")
    };
    RUNTEST(hello);
    return 0;
}