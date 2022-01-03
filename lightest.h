#pragma once

#include <iostream>
#include <vector>

#define DEFTEST(name) void (*name)(Testing&&) = [] (Testing&& testing) 
#define RUNTEST(name) (*name)(Testing( __FILE__, #name ))
#define REPORTTEST() Testing.Report()

class Testing {
    public:
        Testing(const char* file,const char* name) {
            std::cout << " [Begin] " << name << " ==========" << std::endl;
            test.name = name, test.file = file;
            test.failureCount = 0, test.failed = false;
        }
        void Msg(int line,const char* str) {
            std::cout << " [Message] " << test.file << " : " << line << " :  " << str << std::endl;
        }
        void Warn(int line,const char* str) {
            std::cout << " [Warn] " << test.file << " : " << line << " :  " << str << std::endl;
        }
        void Err(int line,const char* str) {
            std::cout << " [Error] " << test.file << " : " << line << ":  " << str << std::endl;
            test.failed = true, test.failureCount++;
        }
        void Fail(int line,const char* str) {
            std::cout << " [Fail] " << test.file << " : " << line << " :  " << str << std::endl;
            test.failed = true, test.failureCount++;
        }
        ~Testing() {
            std::cout << " [End] " << this->name;
            if(this->failed) std::cout << " { " << this->failureCount << " Failure } ";
            std::cout << " ==========" <<std::endl;
            test.push_back(test);
        }
        static void Report() {
            std::cout << "[Report] ==========" << std::endl;
            for(Test item : tests) {
                std::cout << "| " << item.file << " : " << item.name << " :  " <<
                          << item.failureCount << " failure " << std::endl;
            }
            std::cout << "[Report] ==========" << std::endl;
        }
    private:
        class Test {
            public:
                const char* file;
                const char* name;
                unsigned int failureCount;
                bool failed;
        };
        Test test;
        static std::vector<Test> tests;
};

#define MSG(str) testing.Msg(__LINE__,(str))
#define WARN(str) testing.Warn(__LINE__,(str))
#define ERR(str) testing.Err(__LINE__,(str))
#define FAIL(str) testing.Fail(__LINE__,(str))

#define REQUIRE(condition) if(!(condition)) FAIL(" { Require } Didn't pass " #condition)
#define CHECK(condition) if(condition) MSG(" { Check } Pass " #condition); \
                           else FAIL(" { Check } Didn't pass " #condition)
