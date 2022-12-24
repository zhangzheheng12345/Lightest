#include <string>

#include "lightest.h"

CONFIG(Config1) {
	for(;argn > 0; argn--, argc++) {
		if(std::string(*argc) == "--no-color") NO_COLOR();
		if(std::string(*argc) == "--simpler") SIMPLER();
	}
	// NO_COLOR();// N
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