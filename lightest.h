#pragma once

#include <iostream>
#include <time>
#include <vector>

/* ========== Testcase ========== */
#define DEFCASE(name) auto name = [&] (Testcase&& testing)
#define RUNCASE(name) name(Testcase(#name))

class Testcase {
    public:
        Testcase(const char* name) {
            std::cout << " [Begin] " << name << " >>>>>>>>>>>>>>>>>>>>" << std::endl;
            this->name = name;
            this->start = clock();
        }
        ~Testcase() {
            std::cout << " [End  ] " << name << clock() - start << "ms <<<<<<<<<<<<<<<<<<<<" << std::endl;
        }
    private:
        const char* name;
        clock_t start;
};

/* ========== Testing ========== */
#define DEFTEST(name) auto name = [&] (Testing&& testing) 
#define RUNTEST(name) name(Testing( __FILE__, #name ))
#define REPORTTEST() Testing::Report()

class Testing {
    public:
        Testing(const char* file,const char* name) {
            std::cout << " [Begin] " << name << " ==========" << std::endl;
            test.name = name, test.file = file;
            test.failureCount = 0, test.failed = false;
            start = clock();
        }
        void Msg(int line,const char* str) {
            std::cout << " [Msg  ] " << test.file << ":" << line << ": " << str << std::endl;
        }
        void Warn(int line,const char* str) {
            std::cout << " [Warn ] " << test.file << ":" << line << ": " << str << std::endl;
        }
        void Err(int line,const char* str) {
            std::cout << " [Error] " << test.file << ":" << line << ": " << str << std::endl;
            test.failed = true, test.failureCount++;
        }
        void Fail(int line,const char* str) {
            std::cout << " [Fail ] " << test.file << ":" << line << ": " << str << std::endl;
            test.failed = true, test.failureCount++;
        }
        void Log(int line,const char* varname) {
            std::cout << " [Log  ] " << test.file << ":" << line << ": " << varname << " = ";
        }
        ~Testing() {
            clock_t duration = clock() - start;
            std::cout << " [End  ] " << test.name;
            if(test.failed) std::cout << " { " << test.failureCount << " Failure } ";
            std::cout << duration << "ms ==========" << std::endl;
            test.duration = duration;
            tests.push_back(test);
        }
        static void Report() {
            std::cout << "[Report] ==========" << std::endl;
            for(Test item : tests) {
                std::cout << "* " << item.file << ":" << item.name << ": "
                          << item.failureCount << " failure " << item.duration << "ms"<< std::endl;
            }
            std::cout << "[Report] ==========" << std::endl;
            tests.clear();
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
        static std::vector<Test> tests;
};

std::vector<Testing::Test> Testing::tests(0);

/* ========== Logging Macros ========== */
#define MSG(str) testing.Msg(__LINE__,(str))
#define WARN(str) testing.Warn(__LINE__,(str))
#define ERR(str) testing.Err(__LINE__,(str))
#define FAIL(str) testing.Fail(__LINE__,(str))
#define LOG(varname) do { testing.Log(__LINE__,#varname); std::cout << varname <<std::endl; } while(0)

/* ========== Assertion Macros ========== */
#define REQUIRE(condition) ( [&] () { \
                                if(!(condition)) \
                                    FAIL(" { Require } Didn't pass " #condition); \
                                return !(condition); \
                             } () )
#define CHECK(condition) ( [&] () { \
                              if(condition) MSG(" { Check } Pass " #condition); \
                              else FAIL(" { Check } Didn't pass " #condition); \
                              return !(condition); \
                           } () )
