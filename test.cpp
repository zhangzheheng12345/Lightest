#include <iostream>
#include “test.cpp”

int main() {
    DEFTEST(hello) {
        logger.Msg(“hello”);
    }
    RUNTEST(hello)
    return 0;
}