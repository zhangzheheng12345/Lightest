#pragma once

#include <iostream>

#define DEFTEST(name) void (*name)(Testing&&) = [] (Testing&& testing) 
#define RUNTEST(name) (*name)(Testing( #name ));

class Testing {
    public:
        Testing(const char* name) {
            std::cout << " [Begin] " << name << " ==========" << std::endl;
            this->name = name, this->failed = false;
        }
        void Msg(const char* file,int line,const char* str) {
            std::cout << " [Message] " << file << ":" << line << ": " << str << std::endl;
        }
        void Warn(const char* file,int line,const char* str) {
            std::cout << " [Warn] " << file << ":" << line << ": " << str << std::endl;
        }
        void Err(const char* file,int line,const char* str) {
            std::cout << " [Error] " << file << ":" << line << ": " << str << std::endl;
            this->failed = true;
        }
        void Fail(const char* file,int line,const char* str) {
            std::cout << " [Fail] " << file << ":" << line << ": " << str << std::endl;
            this->failed = true;
        }
        ~Testing() {
            std::cout << " [End] " << this->name;
            if(this->failed) std::cout << " { Failed } ";
            std::cout << " ==========" <<std::endl;
        }
    private:
        const char* name;
        bool failed;
};

#define MSG(str) testing.Msg(__FILE__,__LINE__,(str))
#define WARN(str) testing.Warn(__FILE__,__LINE__,(str))
#define ERR(str) testing.Err(__FILE__,__LINE__,(str))
#define FAIL(str) testing.Fail(__FILE__,__LINE__,(str))

#define REQUIRE(condition) if(condition) MSG(" [Require] pass " #condition); \
                           else FAIL(" [Require] didn't pass " #condition)
