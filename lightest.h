#pragma once

#include <iostream>
#include <ctime>
#include <vector>

namespace lightest {
using namespace std;

/* ========== Testing ========== */
#define DEFTEST(name) \
    void name(lightest::Testing&& testing); \
    lightest::Signer signer_ ## name(__FILE__, #name, name); \
    void name(lightest::Testing&& testing)

class Testing {
    public:
        Testing(const char* file, const char* name) {
            cout << "[Begin ] -------------------- " << name << endl;
            test.name = name, test.file = file;
            test.failureCount = 0, test.failed = false;
            start = clock();
        }
        void Msg(int line, const char* str) {
            cout << " | [Msg  ] " << test.file << ":" << line << ": " << str << endl;
        }
        void Warn(int line, const char* str) {
            cout << " | [Warn ] " << test.file << ":" << line << ": " << str << endl;
        }
        void Err(int line, const char* str) {
            cout << " | [Error] " << test.file << ":" << line << ": " << str << endl;
            test.failed = true, test.failureCount++;
        }
        void Fail(int line, const char* str) {
            cout << " | [Fail ] " << test.file << ":" << line << ": " << str << endl;
            test.failed = true, test.failureCount++;
        }
        template<typename T> void Log(int line, const char* varname, T value) {
            cout << " | [Log  ] " << test.file << ":" << line << ": "
                      << varname << " = " << value << endl;
        }
        ~Testing() {
            clock_t duration = clock() - start;
            cout << "[End   ] -------------------- " << test.name;
            if(test.failed) cout << " FAIL" << endl;
            else cout << " PASS" << endl;
            if(test.failed) cout << "  >> FAILURE: " << test.failureCount << endl;
            cout << "  >> TIME: " << duration << "ms" << endl;
            test.duration = duration;
            testsTotal.push_back(test);
        }
        static void ReportTotal() {
            cout << "[Report  ] -------------------- TOTAL" << endl;
            for(Test item : testsTotal) {
                cout << " * " << item.name << ": "
                          << item.failureCount << " failure, " << item.duration << "ms  "
                          << "( " << item.file << " )" << endl;
            }
            cout << " # " << totalFailedTestCount << " failed tests." << endl;
            cout << "[Report  ] -------------------- TOTAL" << endl
                << "Done. " << clock() << "ms used." << endl;
        }
    private:
        class Test {
            public:
                const char* file;
                const char* name;
                unsigned int failureCount;
                bool failed;
                clock_t duration;
        };
        Test test;
        clock_t start; // No need to report.
        static vector<Test> testsTotal;
        static int totalFailedTestCount;
};

vector<Testing::Test> Testing::testsTotal(0);
int Testing::totalFailedTestCount = 0;

/* ========== Signer ========== */

class Signer {
    public:
        Signer(const char* file, const char* name, void (*func)(Testing&&)) {
            signedTestList.push_back({file, name, func});
        }
        static void TestAll() {
            for(auto item : signedTestList) {
                (*item.func)(Testing(item.file, item.name));
            }
            signedTestList.clear();
        }
        /* A signed independence test list */
        typedef struct {
            const char* file;
            const char* name;
            void (*func)(Testing&&);
        } signedTestWrapper;
        static vector<signedTestWrapper> signedTestList;
};
vector<Signer::signedTestWrapper> Signer::signedTestList(0);

}; /* namespace ending */

/* ========== Default main function ========== */

#define MAIN \
    int main() { \
        lightest::Signer::TestAll(); lightest::Testing::ReportTotal(); \
        return 0; }

/* ========== Logging Macros ========== */
#define MSG(str) testing.Msg(__LINE__,(str))
#define WARN(str) testing.Warn(__LINE__,(str))
#define ERR(str) testing.Err(__LINE__,(str))
#define FAIL(str) testing.Fail(__LINE__,(str))
#define LOG(varname) testing.Log(__LINE__,#varname,(varname))

/* ========= Timer Macros =========== */
#define TIMER(sentence) \
    ( [&] () { \
        clock_t start = clock(); \
        (sentence); \
        return clock() - start; \
    } () )
#define AVG_TIMER(sentence, times) \
    ( [&] () { \
        clock_t sum = 0, start; \
        for(unsigned int index = 1; index <= times; index++) { \
            start = clock(); \
            (sentence); \
            sum += clock() - start; \
        } \
        return double(sum) / times; \
    } () )

/* ========== Assertion Macros ========== */
#define REQUIRE(condition) \
    ( [&] () { \
        bool res = !(condition); \
        if(condition) { \
        FAIL("Didn't pass (" #condition ")" ); \
        std::cout << " |\t\t\t{ REQUIRE }" << std::endl; \
        } return res; \
    } () )
#define CHECK(condition) \
    ( [&] () { \
        bool res = condition; \
        if(res) MSG("Pass (" #condition ")" ); \
        else FAIL("Didn't pass (" #condition ")" ); \
        std::cout << " |\t\t\t{ CHECK }" << std::endl; \
        return !(res); \
    } () )
#define REQ_LOG(varname, condition) \
    do { \
        if(REQUIRE(condition)) \
            std::cout << " |\t\t\t#ACTUAL: " #varname " = " << varname << std::endl; \
    } while(0)
#define CHK_LOG(varname, condition) \
    do { \
        if(CHECK(condition)) \
            std::cout << " |\t\t\t#ACTUAL: " #varname " = " << varname << std::endl; \
    } while(0)
