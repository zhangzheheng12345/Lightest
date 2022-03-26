#pragma once

#include <iostream>
#include <fstream>
#include <functional>
#include <ctime>
#include <vector>
#include <string>

/* ========== Testing ========== */
#define DEFTEST(name) std::function<void(Testing&&)>* name = new std::function<void(Testing&&)>; \
                      testing.SignTest(__FILE__, #name, name); \
                      *name = [&] (Testing&& testing)

class Testing {
    public:
        Testing(const char* casename, const char* file, const char* name, const bool global) {
            foreSpace = global ? "" : " ";
            std::cout << foreSpace << "[Begin ] -------------------- " << name << std::endl;
            test.casename = casename, test.name = name, test.file = file;
            test.failureCount = 0, test.failed = false;
            start = clock();
        }
        const char* getForeSpace() const {
            return foreSpace;
        }
        void Msg(int line, const char* str) {
            std::cout << foreSpace << " | [Msg  ] " << test.file << ":" << line << ": " << str << std::endl;
        }
        void Warn(int line, const char* str) {
            std::cout << foreSpace << " | [Warn ] " << test.file << ":" << line << ": " << str << std::endl;
        }
        void Err(int line, const char* str) {
            std::cout << foreSpace << " | [Error] " << test.file << ":" << line << ": " << str << std::endl;
            test.failed = true, test.failureCount++;
        }
        void Fail(int line, const char* str) {
            std::cout << foreSpace << " | [Fail ] " << test.file << ":" << line << ": " << str << std::endl;
            test.failed = true, test.failureCount++;
        }
        template<typename T> void Log(int line, const char* varname, T value) {
            std::cout << foreSpace << " | [Log  ] " << test.file << ":" << line << ": "
                      << varname << " = " << value << std::endl;
        }
        ~Testing() {
            clock_t duration = clock() - start;
            std::cout << foreSpace << "[End   ] -------------------- " << test.name;
            if(test.failed) std::cout << " FAIL" << std::endl;
            else std::cout << " PASS" << std::endl;
            std::cout << foreSpace << "  >> FAILURE: " << test.failureCount << std::endl;
            std::cout << foreSpace << "  >> TIME: " << duration << "ms" << std::endl;
            test.duration = duration;
            testsInCase.push_back(test);
            testsTotal.push_back(test);
        }
        static void ReportInCase() {
            int failedTestCount = 0;
            #define CHK_THEN_PUT if(doCaseReport) std::cout
            CHK_THEN_PUT << " [Report] -------------------- CASE" << std::endl;
            for(Test item : testsInCase) {
                CHK_THEN_PUT << "   * " << item.file << ":" << item.name << ": "
                    << item.failureCount << " failure, " << item.duration << "ms"<< std::endl;
                if(item.failed) failedTestCount++;
            }
            CHK_THEN_PUT << "   # " << failedTestCount << " failed tests." << std::endl;
            totalFailedTestCount += failedTestCount;
            CHK_THEN_PUT << " [Report] -------------------- CASE" << std::endl;
            #undef CHK_THEN_PUT
            testsInCase.clear();
        }
        static void ReportTotal() {
            std::cout << "[Report  ] -------------------- TOTAL" << std::endl;
            for(Test item : testsTotal) {
                std::cout << " * " << item.casename << "." << item.name << ": "
                          << item.failureCount << " failure, " << item.duration << "ms  "
                          << "( " << item.file << " )" << std::endl;
            }
            std::cout << " # " << totalFailedTestCount << " failed tests." << std::endl;
            std::cout << "[Report  ] -------------------- TOTAL" << std::endl;
        }
        static void DoCaseReport() { doCaseReport = true; }
    private:
        class Test {
            public:
                const char* casename;
                const char* file;
                const char* name;
                unsigned int failureCount;
                bool failed;
                clock_t duration;
        };
        Test test;
        clock_t start; // No need to report.
        const char* foreSpace;
        static std::vector<Test> testsInCase;
        static std::vector<Test> testsTotal;
        static int totalFailedTestCount;
        static bool doCaseReport;
};

std::vector<Testing::Test> Testing::testsInCase(0);
std::vector<Testing::Test> Testing::testsTotal(0);
int Testing::totalFailedTestCount = 0;
bool Testing::doCaseReport = false;

/* ========== Testcase ========== */

#define DEFCASE(name) std::function<void(Testcase&&)>* name = new std::function<void(Testcase&&)>; \
                      testing.SignCase(#name, name); \
                      *name = [&] (Testcase&& testing) 

class Testcase {
    public:
        Testcase(const char* name) {
            std::cout << "[Begin   ] " << "==================== " << name << std::endl;
            this->name = name;
            this->start = clock();
        }
        void SignTest(const char* file, const char* name, std::function<void(Testing&&)>* func) {
            signedTestList.push_back({file, name, func});
        }
        ~Testcase() {
            for(auto item : signedTestList) {
                (*item.func)(Testing(name, item.file, item.name, false)); /* These aren't global tests */
                delete item.func;
            }
            Testing::ReportInCase();
            std::cout << "[End     ] " << "==================== " << name << " " << clock() - start << "ms" << std::endl;
        }
    private:
        const char* name;
        clock_t start;
        /* A signed case test list */
        typedef struct {
            const char* file;
            const char* name;
            std::function<void(Testing&&)>* func;
        } signedTestWrapper;
        std::vector<signedTestWrapper> signedTestList;
};

/* ========== Global Case Recorder ========== */

class GlobalSigner {
    public:
        void SignTest(const char* file, const char* name, std::function<void(Testing&&)>* func) {
            signedTestList.push_back({file, name, func});
        }
        void SignCase(const char* name, std::function<void(Testcase&&)>* func) {
            signedCaseList.push_back({name, func});
        }
        void TestAll() {
            for(auto item : signedCaseList) {
                (*item.func)(Testcase(item.name));
                delete item.func;
            }
            for(auto item : signedTestList) {
                (*item.func)(Testing("global", item.file, item.name, true)); /* These are global tests */
                delete item.func;
            }
            signedCaseList.clear(); signedTestList.clear();
        }
        void SetOption(const std::string& k, const std::string& v) {
            if(k == "case_report" && v == "true") Testing::DoCaseReport();
        }
        GlobalSigner() {
            /* Load configurations from lightest.config */
            std::string str;
            std::ifstream file("lightest.config");
            if(file.is_open()) {
                while(!file.eof()) {
                    file >> str;
                    if(!str.length()) break;
                    else if(str[0] == '[') {
                        std::cout << file.get();
                        while(str.find("]") >= str.length()) file >> str;  /* Ignore section label */
                    } else if(str[0] == ';') {
                        while(file.get() != '\n' && !file.eof()) continue;
                    } else {
                        unsigned int i = str.find("=");
                        if(i < str.length() - 1) {
                            SetOption(str.substr(0, i), str.substr(i+1, str.length() - i));
                        } else if(i == str.length() - 1) {
                            std::string key = str.substr(0, str.length() - 1);
                            file >> str;
                            SetOption(key, str);
                        } else {
                            std::string key = str; file >> str;
                            if(str == "=") file >> str;
                            else if(str[0] == '=') str = str.substr(1, str.length() - 1);
                            SetOption(key, str);
                        }
                    }
                }
            }
        }
        ~GlobalSigner() {
            TestAll();
            Testing::ReportTotal();
            std::cout << "Done. " << clock() << "ms used." << std::endl;
        }
    private:
        /* A signed independence test list */
        typedef struct {
            const char* file;
            const char* name;
            std::function<void(Testing&&)>* func;
        } signedTestWrapper;
        std::vector<signedTestWrapper> signedTestList;
        /* A signed test case list */
        typedef struct {
            const char* name;
            std::function<void(Testcase&&)>* func;
        } signedCaseWrapper;
        std::vector<signedCaseWrapper> signedCaseList;
};

GlobalSigner testing;

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
        std::cout << testing.getForeSpace() \
                  << " |\t\t\t{ REQUIRE }" << std::endl; \
        } return res; \
    } () )
#define CHECK(condition) \
    ( [&] () { \
        bool res = condition; \
        if(res) MSG("Pass (" #condition ")" ); \
        else FAIL("Didn't pass (" #condition ")" ); \
        std::cout << testing.getForeSpace() << \
            " |\t\t\t{ CHECK }" << std::endl; \
        return !(res); \
    } () )
#define REQ_LOG(varname, condition) \
    do { \
        if(REQUIRE(condition)) \
            std::cout << testing.getForeSpace() \
                      << " |\t\t\t#ACTUAL: " #varname " = " << varname << std::endl; \
    } while(0)
#define CHK_LOG(varname, condition) \
    do { \
        if(CHECK(condition)) \
            std::cout << testing.getForeSpace() \
                      << " |\t\t\t#ACTUAL: " #varname " = " << varname << std::endl; \
    } while(0)