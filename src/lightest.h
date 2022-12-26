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

#ifdef _WIN_
#include <Windows.h>
#endif

namespace lightest {
using namespace std;

/* ========== Output Color ==========*/

// TODO: use enum class
enum Color {
   Reset = 0, Black = 30, Red = 31, Green = 32, Yellow = 33
};
bool OutputColor = true;

void SetColor(Color color) {
if(OutputColor) {
#if defined(_LINUX_) || defined(_MAC_)
    cout << "\033[" << color << "m";
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
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), winColor);
#endif
}}

/* ========== Data ========== */

// TODO: use enum class
enum FiltLevel {
    ALL = 0, MSG_LOWER, WARN_LOWER, ERR_LOWER // LOG => MSG level, FAIL will always be outputted
};

bool moreOutput = true; // Use SIMPLER() to set to false
FiltLevel filtLevel = ALL; // Use FILTER(level) to set
bool toOutput = true; // Use NO_OUTPUT to set to false

// TODO: use enum class
enum DataType {
    DATA_SET, DATA_MSG, DATA_WARN, DATA_ERR, DATA_FAIL, DATA_LOG, DATA_REQ
};

class Data {
public:
    virtual void Print() const = 0;
    virtual DataType Type() const = 0;
    virtual ~Data() {}
};

class DataSet : public Data {
public:
    DataSet(const char* name) {
        this->name = name, duration = 0, failureCount = 0;
    }
    void Add(Data* son) {
        sons.push_back(son);
    }
    void End(bool failed, clock_t duration, unsigned int failureCount) {
        this->failed = failed, this->duration = duration, this->failureCount = failureCount;
    }
    void PrintSons() const {
		for (const Data* item : sons) {
            item->Print();
        }
	}
    void Print() const {
        cout << "[Begin ] " << name << endl;
        PrintSons();
        cout << "[End   ] " << name;
        if (failed) { SetColor(Red); cout << " FAIL" << endl; }
        else { SetColor(Green); cout << " PASS" << endl; }
        SetColor(Reset);
        if (moreOutput) {
            if (failed) cout << "  >> FAILURE: " << failureCount << endl;
            cout << "  >> TIME: " << duration / CLOCKS_PER_SEC * 1000 << "ms" << endl;
        }
	}
    DataType Type() const { return DATA_SET; }
    // public for data processing
    const char* name;
    bool failed;
    unsigned int failureCount;
    clock_t duration;
    vector<Data*> sons;
};

class DataUnit {
public:
    const char* file;
    unsigned int line;
};
class DataMsg : public Data, public DataUnit {
public:
    DataMsg(const char* file, unsigned int line, const char* str) {
        this->file = file, this->line = line, this->str = str;
    }
    void Print() const {
		if(filtLevel < MSG_LOWER) {
            cout << "    "; SetColor(Green); cout << "[Msg  ] "; SetColor(Reset);
            cout << file << ":" << line << ": " << str << endl;
		}
    }
    DataType Type() const { return DATA_MSG; }
    const char* str;
};
class DataWarn : public Data, public DataUnit{
public:
    DataWarn(const char* file, unsigned int line, const char* str) {
        this->file = file, this->line = line, this->str = str;
    }
    void Print() const {
		if(filtLevel < WARN_LOWER) {
            cout << "    "; SetColor(Yellow); cout << "[Warn ] "; SetColor(Reset);
            cout << file << ":" << line << ": " << str << endl;
		}
    }
    DataType Type() const { return DATA_WARN; }
    const char* str;
};
class DataError : public Data, public DataUnit {
public:
    DataError(const char* file, unsigned int line, const char* str) {
        this->file = file, this->line = line, this->str = str;
    }
    void Print() const {
		if(filtLevel < ERR_LOWER) {
            cout << "    "; SetColor(Red); cout << "[Error] "; SetColor(Reset);
            cout << file << ":" << line << ": " << str << endl;
		}
    }
    DataType Type() const { return DATA_ERR; }
    const char* str;
};
class DataFail : public Data, public DataUnit {
public:
    DataFail(const char* file, unsigned int line, const char* str) {
        this->file = file, this->line = line, this->str = str;
    }
    void Print() const {
        cout << "    "; SetColor(Red); cout << "[Fail ] "; SetColor(Reset);
        cout << file << ":" << line << ": " << str << endl;
    }
    DataType Type() const { return DATA_FAIL; }
    const char* str;
};
template<class T> class DataLog : public Data, public DataUnit {
public:
    DataLog(const char* file, unsigned int line, const char* varName, const T& value_) : value(value_) {
        this->file = file, this->line = line, this->varName = varName;
    }
    void Print() const {
		if(filtLevel < MSG_LOWER) {
            cout << "    "; SetColor(Green); cout << "[Log  ] "; SetColor(Reset);
            cout << file << ":" << line << ": "
                << varName << " = " << value << endl;
		}
    }
    DataType Type() const { return DATA_LOG; }
    const char* varName;
    const T value;
};
template<class T> class DataReq : public Data, public DataUnit{
public:
    DataReq(const char* file, unsigned int line,
        const T& actual_, const T& expected_, const char* operator_, bool failed_)
            : actual(actual_), expected(expected_), failed(failed_) {
        this->file = file, this->line = line, this->operator_ = operator_;
    }
    void Print() const {
        if(failed) {
            DataFail(file, line, "Req failed").Print();
            cout << "        + ACTUAL: " << actual << endl;
            cout << "        + EXPECTED: " << operator_ << " " << expected << endl;
        }
    }
    DataType Type() const { return DATA_REQ; }
    const T actual, expected;
    const char* operator_;
    const bool failed;
};

/* ========== Signer ========== */

class Register {
public:
    Register(const char* name) {
        testData = new DataSet(name);
        for (unsigned int i = 0; i < 3; i++) registerList[i] = vector<signedFuncWrapper>(0);
    }
    Register() {
        testData = NULL;
        for (unsigned int i = 0; i < 3; i++) registerList[i] = vector<signedFuncWrapper>(0);
    }
    typedef struct {
        DataSet* testData;
        int argn;
        char** argc;
    } Context;
    void Add(const char* name, void (*callerFunc)(Context&), unsigned int level) {
        registerList[level].push_back({ name, callerFunc });
    }
    void RunRegistered() {
        Context ctx = Context{testData, argn, argc};
        for(unsigned int i = 0; i < 3; i++) {
            for (const signedFuncWrapper& item : registerList[i]) {
                (*item.callerFunc)(ctx);
            }
        }
    }
    static void SetArg(int argn, char** argc) {
		Register::argn = argn, Register::argc = argc;
	}
    DataSet* testData;
private:
    typedef struct {
        const char* name;
        void (*callerFunc)(Register::Context&);
    } signedFuncWrapper;
    vector<signedFuncWrapper> registerList[3];
    static int argn;
    static char** argc;
};
int Register::argn = 0;
char** Register::argc = nullptr;

Register globalRegister("");

class Registering {
public:
    Registering(Register& reg,
        const char* name, void (*callerFunc)(Register::Context&), unsigned int level) {
        reg.Add(name, callerFunc, level);
    }
};

/* ========== Testing ========== */

class Testing {
    public:
        Testing(const char* name) {
            reg = Register(name);
            failureCount = 0, failed = false;
            start = clock();
        }
        void Msg(const char* file, int line, const char* str) {
            reg.testData->Add(new DataMsg(file, line, str));
        }
        void Warn(const char* file, int line, const char* str) {
            reg.testData->Add(new DataWarn(file, line, str));
        }
        void Err(const char* file, int line, const char* str) {
            reg.testData->Add(new DataError(file, line, str));
            failed = true, failureCount++;
        }
        void Fail(const char* file, int line, const char* str) {
            reg.testData->Add(new DataFail(file, line, str));
            failed = true, failureCount++;
        }
        template<typename T> void Log(const char* file, int line, const char* varName, const T& value) {
            reg.testData->Add(new DataLog<T>(file, line, varName, value));
        }
        template<typename T> void Req(const char* file, int line,
            const T& actual, const T& expected, const char* operator_, bool failed) {
            reg.testData->Add(new DataReq<T>(file, line, actual, expected, operator_, failed));
			this->failed = !failed;
        }
        DataSet* GetData() {
            return reg.testData;
        }
        ~Testing() {
			reg.RunRegistered();
            reg.testData->End(failed, clock() - start, failureCount);
        }
    private:
        clock_t start; // No need to report.
        unsigned int failureCount;
        bool failed;
        Register reg;
};

} // namespace ending

/* ========== Registing macros ========== */

#define CONFIG(name) \
    void name(int argn, char** argc); \
    void call_ ## name(lightest::Register::Context& ctx){ \
        name(ctx.argn, ctx.argc); \
    } \
    lightest::Registering registering_ ## name(lightest::globalRegister, #name, call_ ## name, 0); \
    void name(int argn, char** argc)
#define TEST(name) \
    void name(lightest::Testing& testing); \
    void call_ ## name(lightest::Register::Context& ctx){ \
        lightest::Testing testing(#name); \
        name(testing); \
        ctx.testData->Add(testing.GetData()); \
    } \
    lightest::Registering registering_ ## name(lightest::globalRegister, #name, call_ ## name, 1); \
    void name(lightest::Testing& testing)
#define DATA(name) \
    void name(const lightest::DataSet* data); \
    void call_ ## name(lightest::Register::Context& ctx){ \
        name(ctx.testData); \
    } \
    lightest::Registering registering_ ## name(lightest::globalRegister, #name, call_ ## name, 2); \
    void name(const lightest::DataSet* data)

/* ========== Configuration macros ========== */

#define SIMPLER() lightest::moreOutput = false;
#define NO_COLOR() lightest::OutputColor = false;
#define FILTER(level) lightest::filtLevel = level;
#define NO_OUTPUT() lightest::toOutput = false;

/* ========== Main ========== */

int main(int argn, char* argc[]) {
	lightest::Register::SetArg(argn, argc);
	lightest::globalRegister.RunRegistered();
	if(lightest::toOutput) {
		lightest::globalRegister.testData->PrintSons();
	}
	return 0;
}

/* ========== Logging Macros ========== */

#define MSG(str) testing.Msg(__FILE__, __LINE__,(str))
#define WARN(str) testing.Warn(__FILE__, __LINE__,(str))
#define ERR(str) testing.Err(__FILE__, __LINE__, (str))
#define FAIL(str) testing.Fail(__FILE__, __LINE__, (str))
#define LOG(varName) testing.Log(__FILE__, __LINE__, #varName, (varName))

/* ========= Timer Macros =========== */

// unit: minisecond(ms)
#define TIMER(sentence) \
    ( [&] () { \
        clock_t start = clock(); \
        (sentence); \
        return (clock() - start) / CLOCKS_PER_SEC * 1000; \
    } () )
#define AVG_TIMER(sentence, times) \
    ( [&] () { \
        clock_t sum = 0, start; \
        for(unsigned int index = 1; index <= times; index++) { \
            start = clock(); \
            (sentence); \
            sum += clock() - start; \
        } \
        return double(sum) / times / CLOCKS_PER_SEC * 1000; \
    } () )

/* ========== Assertion Macros ========== */

// true => pass, false => fail
#define REQ(actual, operator, expected) \
    ( [&] () -> bool { \
        bool res = (actual) operator (expected); \
        testing.Req(__FILE__, __LINE__, actual, expected, #operator, !res); \
        return res; \
    } ) ()

// condition must be true or stop currnet test
#define MUST(condition) do { if(!(condition)) { FAIL("A must didn't pass"); return; } } while(0) 

#undef _LINUX_
#undef _WIN_
#undef _MAC_

#endif