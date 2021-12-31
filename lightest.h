#pragma once

#include <iostream>

#define DEFTEST(name) void (*name)(Logger&&) = [] (Logger&& logger) 
#define RUNTEST(name) (*name)(Logger( #name ));

class Logger {
    public:
        Logger(const char* name) {
            std::cout << " [Begin] " << name << " ==========" << std::endl;
            this->name = name, this->failed = false; }
        void Msg(const char* str) { std::cout << " [Message] " << str << std::endl; }
        void Fail(const char* str) { std::cout << " [Fail] " << str << std::endl; this->failed = true; }
        ~Logger() {
            std::cout << " [End] " << this->name;
            if(this->failed) std::cout << " [Failed] ";
            std::cout << " ==========" <<std::endl; }
    private:
        const char* name;
        bool failed;
};

#define MSG(str) logger.Msg((str))