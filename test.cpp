#include "lightest.h"

MAIN
// LESS_MAIN

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

DEFTEST(TestAssertionMacors) {
    int a = 0, b = 0, c = 1;
    REQUIRE(a == b); REQUIRE(a != c);
    CHECK(a == b); CHECK(a != c);
    REQ_LOG(a, b, a == b); REQ_LOG(a, c, a!= c);
    CHK_LOG(a, b, a == b); CHK_LOG(a, c, a != c);
    REQ_OP(a, b, ==); REQ_OP(a, c, !=);
    CHK_OP(a, b, ==); CHK_OP(a, c, !=);
    int arr1[] = {1, 2, 3, 4}, arr2[] = {1, 2, 3, 4}, arr3[] = {1, 2, 3}, arr4[] = {1, 2, 3, 5};
    REQ_ARRAY(arr1, arr2, 4, 4, ==); // PASS
    REQ_ARRAY(arr1, arr3, 4, 3, ==); REQ_ARRAY(arr1, arr4, 4, 4, ==); // FAIL
    CHK_ARRAY(arr1, arr2, 4, 4, ==); // PASS
    CHK_ARRAY(arr1, arr3, 4, 3, ==); CHK_ARRAY(arr1, arr4, 4, 4, ==); // FAIL
}
