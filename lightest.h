#pragma once

#include <iostream>
#include <functional>
#include <ctime>
#include <vector>

/* ========== Testing ========== */
#define DEFTEST(name) std::function<void(Testing&&)>* name = new std::function<void(Testing&&)>; \
                      testing.SignTest(__FILE__, #name, name); \
                      *name = [&] (Testing&& testing) 

class Testing {
    public:
        Testing(const char* file,const char* name) {
            std::cout << " [Begin] -------------------- " << name << std::endl;
            test.name = name, test.file = file;
            test.failureCount = 0, test.failed = false;
            start = clock();
        }
        void Msg(int line,const char* str) {
            std::cout << "  | [Msg  ] " << test.file << ":" << line << ": " << str << std::endl;
        }
        void Warn(int line,const char* str) {
            std::cout << "  | [Warn ] " << test.file << ":" << line << ": " << str << std::endl;
        }
        void Err(int line,const char* str) {
            std::cout << "  | [Error] " << test.file << ":" << line << ": " << str << std::endl;
            test.failed = true, test.failureCount++;
        }
        void Fail(int line,const char* str) {
            std::cout << "  | [Fail ] " << test.file << ":" << line << ": " << str << std::endl;
            test.failed = true, test.failureCount++;
        }
        template<typename T> void Log(int line,const char* varname,T value) {
            std::cout << "  | [Log  ] " << test.file << ":" << line << ": "
                      << varname << " = " << value << std::endl;
        }
        ~Testing() {
            clock_t duration = clock() - start;
            std::cout << " [End   ] -------------------- " << test.name;
            if(test.failed) std::cout << " FAIL" << std::endl;
            else std::cout << " PASS" << std::endl;
            std::cout << "  FAILURE: " << test.failureCount << std::endl;
            std::cout << "  TIME: " << duration << "ms" << std::endl;
            test.duration = duration;
            testsInCase.push_back(test);
            testsTotal.push_back(test);
        }
        static void ReportInCase() {
            std::cout << " [Report] -------------------- CASE" << std::endl;
            for(Test item : testsInCase) {
                std::cout << "   * " << item.file << ":" << item.name << ": "
                          << item.failureCount << " failure, " << item.duration << "ms"<< std::endl;
            }
            std::cout << " [Report] -------------------- CASE" << std::endl;
            testsInCase.clear();
        }
        static void ReportTotal() {
            std::cout << "[Report] -------------------- TOTAL" << std::endl;
            for(Test item : testsTotal) {
                std::cout << " * " << item.file << ":" << item.name << ": "
                          << item.failureCount << " failure, " << item.duration << "ms"<< std::endl;
            }
            std::cout << "[Report] -------------------- TOTAL" << std::endl;
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
        static std::vector<Test> testsInCase;
        static std::vector<Test> testsTotal;
};

std::vector<Testing::Test> Testing::testsInCase(0);
std::vector<Testing::Test> Testing::testsTotal(0);

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
                (*item.func)(Testing(item.file, item.name));
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
        ~GlobalSigner() {
            for(auto item : signedCaseList) {
                (*item.func)(Testcase(item.name));
                delete item.func;
            }
            for(auto item : signedTestList) {
                (*item.func)(Testing(item.file, item.name));
                delete item.func;
            }
            std::cout << std::endl;
            Testing::ReportTotal();
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

/* ========== Assertion Macros ========== */
#define REQUIRE(condition) ( [&] () { \
                                bool res = !(condition); \
                                if(condition) { \
                                    FAIL("Didn't pass " #condition); \
                                    std::cout << "  |\t\t\t{ REQUIRE }" << std::endl; }\
                                return res; \
                             } () )
#define CHECK(condition) ( [&] () { \
                              bool res = condition; \
                              if(res) MSG("Pass " #condition); \
                              else FAIL("Didn't pass " #condition); \
                              std::cout << "  |\t\t\t{ CHECK }" << std::endl; \
                              return !(res); \
                           } () )
#define REQ_LOG(varname, condition) do{ \
                                        if(REQUIRE(condition)) \
                                        std::cout << "  |\t\t\t#ACTUAL: " #varname " = " << varname << std::endl; \
                                    }while(0)
#define CHK_LOG(varname, condition) do{ \
                                        if(CHECK(condition)) \
                                        std::cout << "  |\t\t\t#ACTUAL: " #varname " = " << varname << std::endl; \
                                    }while(0)