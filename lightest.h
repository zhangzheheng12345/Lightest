#pragma once

#include <iostream>
#include <vector>

#define DEFTEST(name) void (*name)(Testing&&) = [] (Testing&& testing) 
#define RUNTEST(name) (*name)(Testing( __FILE__, #name ))

class Testing {
    public:
        Testing(const char* file,const char* name) {
            std::cout << " [Begin] " << name << " ==========" << std::endl;
            this->name = name, this->file = file;
            this->failureCount = 0, this->failed = false;
        }
        void Msg(int line,const char* str) {
            std::cout << " [Message] " << file << ":" << line << ": " << str << std::endl;
        }
        void Warn(int line,const char* str) {
            std::cout << " [Warn] " << file << ":" << line << ": " << str << std::endl;
        }
        void Err(int line,const char* str) {
            std::cout << " [Error] " << file << ":" << line << ": " << str << std::endl;
            this->failed = true, this->failureCount++;
        }
        void Fail(int line,const char* str) {
            std::cout << " [Fail] " << file << ":" << line << ": " << str << std::endl;
            this->failed = true, this->failureCount++;
        }
        ~Testing() {
            std::cout << " [End] " << this->name;
            if(this->failed) std::cout << " { " << this->failureCount << " Failure } ";
            std::cout << " ==========" <<std::endl;
        }
    private:
        const char* file;
        const char* name;
        unsigned int failureCount;
        bool failed;
};

#define MSG(str) testing.Msg(__LINE__,(str))
#define WARN(str) testing.Warn(__LINE__,(str))
#define ERR(str) testing.Err(__LINE__,(str))
#define FAIL(str) testing.Fail(__LINE__,(str))

#define REQUIRE(condition) if(!(condition)) FAIL(" { Require } Didn't pass " #condition)
#define CHECK(condition) if(condition) MSG(" { Check } Pass " #condition); \
                           else FAIL(" { Check } Didn't pass " #condition)
