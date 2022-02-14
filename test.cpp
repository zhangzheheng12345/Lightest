#include "lightest.h" // Just include lightest.h

int main() {
    DEFCASE(case1) {
        DEFTEST(loggings) {
            MSG("msg");
            WARN("warn");
            ERR("err");
            FAIL("fail");
        };
        DEFTEST(assertions) {
            int a = 2;
            if(CHECK(1<=a)) LOG(a);
            if(CHECK(4<a)) LOG(a);
            if(REQUIRE(1<=a)) LOG(a);
            if(REQUIRE(4<a)) LOG(a);
            REQ_LOG(a, 4<a); // Same as the first one
            CHK_LOG(a, 4<a); // Same as the second one
        };
    };
    return 0;
}