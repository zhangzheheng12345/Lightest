#include "lightest.h"

// MAIN
// LESS_MAIN
int main() {
    EXCEPT(Except1);
    EXCEPT(Except2);
    TESTALL();
    REPORT();
}

DEFTEST(Except1) {
    ERR("Except1 run");
}
DEFTEST(Except2) {
    ERR("Except2 run");
}

DEFTEST(TestOutputMacros) {
    MSG("msg");
    WARN("warn");
    ERR("error");
    FAIL("fail");
    int a = 100;
    LOG(a);
}

DEFTEST(TestTimerMacors) {
    int i = 0;
    LOG(TIMER(i++));
    LOG(AVG_TIMER(i++, 100));
}

DEFTEST(TestThrow) {
    throw "Error from TestThrow";
}

DEFTEST(TestAssertionMacors) {
    int a = 0, b = 0, c = 1;
    REQUIRE(a == b); REQUIRE(a != c);
    REQ_LOG(a, b, a == b); REQ_LOG(a, c, a!= c);
    REQ_OP(a, b, ==); REQ_OP(a, c, !=);
    int arr1[] = {1, 2, 3, 4}, arr2[] = {1, 2, 3, 4}, arr3[] = {1, 2, 3}, arr4[] = {1, 2, 3, 5};
    MUST(REQ_ARRAY(arr1, arr2, 4, 4, ==)); // PASS
    REQ_ARRAY(arr1, arr3, 4, 3, ==); REQ_ARRAY(arr1, arr4, 4, 4, ==); // FAIL
    //  MUST(REQUIRE(1 == 2)); // FAIL & Aborted
}
