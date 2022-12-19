#include "lightest.h"

MAIN
// LESS_MAIN

CONFIG(Config1) {	
	// NO_COLOR();
	// SIMPLER();
	// FILTER(MSG_LOWER);
	// FILTER(WARN_LOWER);
	// FILTER(ERR_LOWER);
}

TEST(TestOutputMacros) {
    MSG("msg");
    WARN("warn");
    ERR("error");
    FAIL("fail");
    int a = 100;
    LOG(a);
}

TEST(TestTimerMacoros) {
    int i = 0;
    LOG(TIMER(i++));
    LOG(AVG_TIMER(i++, 100));
}

TEST(TestAssertionMacors) {
    int a = 0, b = 0, c = 1;
	REQ(a, >, b); // Test fail
    REQ(a, ==, b);
	REQ(b, !=, c);
    //  MUST(REQ(a, ==, c)); // FAIL & Aborted
}