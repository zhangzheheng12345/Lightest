/********************
| Lightest |
This is the core file of this library, which provides a lightest C++ unit test framework.
MIT licensed.
Github Repo: https://github.com/zhangzheheng12345/Lightest
Author's Github: https://github.com/zhangzheheng12345
********************/

#ifndef _LIGHTEST_H_
#define _LIGHTEST_H_

#if defined(__unix__) || defined(__unix) || defined(__linux__)
#define _LINUX_
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#define _WIN_
#elif defined(__APPLE__) || defined(__MACH__)
#define _MAC_
#else
#warning Unknown platform to Lightest will cause no outputing color
#endif

#include <iostream>
#include <ctime>
#include <vector>
#include <set>
#include <exception>

#ifdef _WIN_
#include <Windows.h>
#endif

namespace lightest {
using namespace std;

/* ========== Output Color ==========*/

enum Color {
   Reset = 0, Black = 30, Red = 31, Green = 32, Yellow = 33
};
bool OutputColor = true;

void SetColor(Color color, bool whiteBg = false) {
if(OutputColor) {
#if defined(_LINUX_) || defined(_MAC_)
    cout << "\033[" << color << "m";
    if(whiteBg) cout << "\033[47m";
#elif defined(_WIN_)
    unsigned int winColor;
    switch (color) {
    case Reset: winColor = 0x07; break;
    case Black: winColor = 0x00;
    case Red: winColor = 0x0c; break;
    case Green: winColor = 0x0a; break;
    case Yellow: winColor = 0x0e; break;
    default: winColor = 0x07; break;
    }
    if(whiteBg) winColor += 0xf0;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), winColor);
#endif
}}

/* ========== Testing ========== */

enum FiltLevel {
    ALL = 0, MSG_LOWER, WARN_LOWER, ERR_LOWER // LOG => MSG level, Fail will always be outputted
};

class Testing {
    public:
        Testing(const char* file, const char* name) {
            cout << "[Begin ] =====> " << name << " ----" << endl;
            test.name = name, test.file = file;
            test.failureCount = 0, test.failed = false;
            start = clock();
        }
        static void Msg(const char* file, int line, const char* str) {
            if(level < MSG_LOWER) {
                cout << " | "; SetColor(Green); cout << "[Msg  ] "; SetColor(Reset);
                cout << file << ":" << line << ": " << str << endl;
            }
        }
        static void Warn(const char* file, int line, const char* str) {
            if(level < WARN_LOWER) {
                cout << " | "; SetColor(Yellow); cout << "[Warn ] "; SetColor(Reset);
                cout << file << ":" << line << ": " << str << endl;
            }
        }
        void Err(int line, const char* str) {
            if(level < ERR_LOWER) {
                cout << " | "; SetColor(Red); cout << "[Error] "; SetColor(Reset);
                cout << test.file << ":" << line << ": " << str << endl;
            }
            test.failed = true, test.failureCount++;
            failedTestCount++;
        }
        void Fail(int line, const char* str) {
            cout << " | "; SetColor(Red); cout << "[Fail ] "; SetColor(Reset);
            cout << test.file << ":" << line << ": " << str << endl;
            test.failed = true, test.failureCount++;
            failedTestCount++;
        }
        template<typename T> static void Log(const char* file, int line, const char* varname, T value) {
            if(level < MSG_LOWER) {
                cout << " | "; SetColor(Green); cout << "[Log  ] "; SetColor(Reset);
                cout << file << ":" << line << ": "
                      << varname << " = " << value << endl;
            }
        }
        template<typename T> void Addition(const char * kind, const char* varname, T value) {
            cout << " |   |-> ";
            SetColor(Black, true); cout << kind << ":"; SetColor(Reset);
            cout << " " << varname << " = " << value << endl;
        }
        ~Testing() {
            clock_t duration = clock() - start;
            cout << "[End   ] =====> " << test.name;
            if(test.failed) { SetColor(Red); cout << " FAIL" << endl; }
            else { SetColor(Green); cout << " PASS" << endl; }
            SetColor(Reset);
            if(more) {
                if(test.failed) cout << "  >> FAILURE: " << test.failureCount << endl;
                cout << "  >> TIME: " << duration << "ms" << endl;
            }
            test.duration = duration;
            testsTotal.push_back(test);
        }
        static void Simpler() {
            more = false;
        }
        static void Filter(FiltLevel filt) {
            level = filt;
        }
        static void ReportTotal() {
            cout << "[Report  ] --------------------" << endl;
            unsigned int passedTestCount = 0, failureSum = 0;
            for(Test item : testsTotal) {
                if(item.failed) { SetColor(Red); failureSum += item.failureCount; }
                else { SetColor(Green); passedTestCount++; }
                cout << " * "; SetColor(Reset);
                cout << item.name << ": " << item.failureCount << " failure, " << item.duration << "ms  "
                    << "( " << item.file << " )" << endl;
            }
            cout << " # [ "; SetColor(Green);
            cout << passedTestCount; SetColor(Reset);
            cout << " / " << testsTotal.size() << " ] passed" << endl;
            if(failureSum != 0) {
                cout << " # { "; SetColor(Red);
                cout << failureSum; SetColor(Reset);
                cout << " } failures occurred" << endl;
            }
            cout << "[Report  ] --------------------" << endl
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
        static int failedTestCount;
        static bool more;
        static FiltLevel level;
};

vector<Testing::Test> Testing::testsTotal(0);
int Testing::failedTestCount = 0;
bool Testing::more = true;
FiltLevel Testing::level = ALL;

#define TEST(name) \
    void name(lightest::Testing& testing); \
    lightest::Register register_ ## name(__FILE__, #name, name); \
    void name(lightest::Testing& testing)
#define FILTER(level) lightest::Testing::Filter(level)

/* ========== Signer ========== */

class Register {
    public:
        Register(const char* file, const char* name, void (*func)(Testing&)) {
            signedTestList.push_back({file, name, func});
        }
        static void TestAll() {
            for(signedTestWrapper item : signedTestList) {
                if(!excepts.count(item.name)) {
                    Testing testing = Testing(item.file, item.name);
                    try {
                        (*item.func)(testing);
                    } catch(exception& err) {
                        if(allThrow) throw err;
                        else {
                            testing.Err(-1, err.what());
                            cout << " |   | -> !!! UNCAUGHT ERROR !!!" << endl;
                        }
                    } catch(const char* err) {
                        if(allThrow) throw err;
                        else {
                            testing.Err(-1, err);
                            cout << " |   | -> !!! UNCAUGHT ERROR !!!" << endl;
                        }
                    } catch(...) {
                        if(allThrow) throw;
                        else {
                            testing.Err(-1, "Unknown type error");
                            cout << " |   | -> !!! UNCAUGHT ERROR !!!" << endl;
                        }
                    }
                }
            }
            signedTestList.clear();
        }
        static void Except(const char* name) {
            excepts.insert(name);
        }
        static void AllThrow() {
            allThrow = true;
        }
        /* A signed independence test list */
        typedef struct {
            const char* file;
            const char* name;
            void (*func)(Testing&);
        } signedTestWrapper;
        static vector<signedTestWrapper> signedTestList;
        static set<const char*> excepts;
        static bool allThrow;
};
vector<Register::signedTestWrapper> Register::signedTestList(0);
set<const char*> Register::excepts;
bool Register::allThrow = false;

} /* namespace ending */

/* ========== Default main functions ========== */

#define EXCEPT(name) lightest::Register::Except( #name )
#define TESTALL() lightest::Register::TestAll()
#define REPORT() lightest::Testing::ReportTotal()
#define SIMPLER() lightest::Testing::Simpler()
#define ALL_THROW() lightest::Register::AllThrow()
#define NO_COLOR() lightest::OutputColor = false;

#define MAIN \
    int main() { \
        TESTALL(); REPORT(); \
        return 0; }
#define LESS_MAIN \
    int main() { \
        SIMPLER(); FILTER(lightest::MSG_LOWER); TESTALL(); \
        return 0; }

/* ========== Logging Macros ========== */

#define MSG(str) lightest::Testing::Msg(__FILE__, __LINE__,(str))
#define WARN(str) lightest::Testing::Warn(__FILE__, __LINE__,(str))
#define ERR(str) testing.Err(__LINE__, (str))
#define FAIL(str) testing.Fail(__LINE__, (str))
#define LOG(varname) lightest::Testing::Log(__FILE__, __LINE__, #varname, (varname))

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

// return false; => failed | return true; => passed
#define REQUIRE(condition) \
    ( [&] () -> bool { \
        bool res = !(condition); \
        if(res) \
            FAIL("Didn't pass (" #condition ")" ); \
        return !res; \
    } () )

#define PUT_EXP_ACT(expected, actual) \
    do { testing.Addition("EXPECTED", #expected, expected); testing.Addition("ACTUAL", #actual, actual); } while(0)
#define REQ_LOG(expected, actual, condition) \
    ( [&] () -> bool { \
        if(!REQUIRE(condition)) { \
            PUT_EXP_ACT((expected), (actual)); return false; \
        } return true; \
    } () )
#define REQ_OP(expected, actual, operator) \
    ( [&] () -> bool { \
        if(!REQUIRE((expected) operator (actual))) { \
            PUT_EXP_ACT((expected), (actual)); return false; \
        } return true; \
    } () )
#define REQ_ARR(expected, actual, expLen, actLen, operator) \
    ( [&]() -> bool { \
        if(expLen != actLen) { \
            FAIL("<ARRAY> Lengths aren't equal"); \
            testing.Addition("EXPECTED", "expected length", expLen); \
            testing.Addition("ACTUAL", "actual length", actLen); \
            return false; \
        } \
        bool failed = true; \
        for(unsigned int i = 0; i < expLen; i++) { \
            if(!REQUIRE(expected[i] operator actual[i])) { \
                testing.Addition("EXPECTED", #expected "[i]", expected[i]); \
                testing.Addition("ACTUAL", #actual "[i]", actual[i]); \
                testing.Addition("INDEX", "", i); \
                failed = false; \
            } \
        } \
        return failed; \
    } )() // Call lambda

// condition must be true
#define MUST(condition) do { bool var = (condition); \
    if(!var) { FAIL("A must didn't pass"); abort(); } } while(0)

#undef _LINUX_
#undef _WIN_
#undef _MAC_

#endif