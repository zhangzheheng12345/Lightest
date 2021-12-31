#include <iostream>
#include "lightest.h"

int main() {
    DEFTEST(hello) {
        logger.Fail("hello");
    };
    RUNTEST(hello);
    return 0;
}