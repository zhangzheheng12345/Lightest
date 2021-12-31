#include <iostream>
#include "lightest.h"

int main() {
    DEFTEST(hello) {
        logger.Msg("hello");
    };
    RUNTEST(hello);
    return 0;
}