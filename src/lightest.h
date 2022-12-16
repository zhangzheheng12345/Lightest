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
#include <exception>
#include <assert.h>

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

enum FiltLevel {
    ALL = 0, MSG_LOWER, WARN_LOWER, ERR_LOWER // LOG => MSG level, Fail will always be outputted
};

bool moreOutput = true; // Use SIMPLER() to set to false
FiltLevel filtLevel = ALL; // 

class Data {
public:
    virtual void Print() const = 0;
    virtual ~Data() {}
};

class DataSet : public Data {
public:
    DataSet(const char* file, const char* name) {
        this->file = file, this->name = name;
    }
    void Add(Data* son) {
        sons.push_back(son);
    }
    void End(bool failed, clock_t duration, unsigned int failureCount) {
        this->failed = failed, this->duration = duration, this->failureCount = failureCount;
    }
    void Print() const {
        cout << "[Begin ] " << name << endl;
        for (const Data* item : sons) {
            item->Print();
        }
        cout << "[End   ] " << name << endl;
        if (failed) { SetColor(Red); cout << " FAIL" << endl; }
        else { SetColor(Green); cout << " PASS" << endl; }
        SetColor(Reset);
        if (moreOutput) {
            if (failed) cout << "  >> FAILURE: " << failureCount << endl;
            cout << "  >> TIME: " << duration / CLOCKS_PER_SEC * 1000 << "ms" << endl;
        }
    }
private:
    const char* file, *name;
    bool failed;
    unsigned int failureCount;
    clock_t duration;
    vector<Data*> sons;
};

class DataMsg : public Data {
public:
    DataMsg(const char* file, unsigned int line, const char* str) {
        this->file = file, this->line = line, this->str = str;
    }
    void Print() const {
        cout << "    "; SetColor(Green); cout << "[Msg  ] "; SetColor(Reset);
        cout << file << ":" << line << ": " << str << endl;
    }
private:
    const char* file;
    unsigned int line;
    const char* str;
};
class DataWarn : public Data {
public:
    DataWarn(const char* file, unsigned int line, const char* str) {
        this->file = file, this->line = line, this->str = str;
    }
    void Print() const {
        cout << "    "; SetColor(Yellow); cout << "[Warn ] "; SetColor(Reset);
        cout << file << ":" << line << ": " << str << endl;
    }
private:
    const char* file;
    unsigned int line;
    const char* str;
};
class DataError : public Data {
public:
    DataError(const char* file, unsigned int line, const char* str) {
        this->file = file, this->line = line, this->str = str;
    }
    void Print() const {
        cout << "    "; SetColor(Red); cout << "[Error] "; SetColor(Reset);
        cout << file << ":" << line << ": " << str << endl;
    }
private:
    const char* file;
    unsigned int line;
    const char* str;
};
class DataFail : public Data {
public:
    DataFail(const char* file, unsigned int line, const char* str) {
        this->file = file, this->line = line, this->str = str;
    }
    void Print() const {
        cout << "    "; SetColor(Red); cout << "[Fail ] "; SetColor(Reset);
        cout << file << ":" << line << ": " << str << endl;
    }
private:
    const char* file;
    unsigned int line;
    const char* str;
};
template<class T> class DataLog : public Data {
public:
    DataLog(const char* file, unsigned int line, const char* varName, T value) {
        this->file = file, this->line = line, this->varName = varName, this->value = value;
    }
    void Print() const {
        cout << "    "; SetColor(Green); cout << "[Log  ] "; SetColor(Reset);
        cout << file << ":" << line << ": "
            << varName << " = " << value << endl;
    }
private:
    const char* file;
    unsigned int line;
    const char* varName;
    T value;
};
// TODO: DataReq, DataReqOp, DataReqArr

/* ========== Signer ========== */

class Register {
public:
    Register(const char* file, const char* name) {
        testData = new DataSet(file, name);
        for (unsigned int i = 0; i < 3; i++) registerList[i] = vector<signedFuncWrapper>(0);
    }
    Register() {
        testData = NULL;
        for (unsigned int i = 0; i < 3; i++) registerList[i] = vector<signedFuncWrapper>(0);
    }
    typedef struct {
        DataSet* testData;
    } Context;
    void Add(const char* file, const char* name, void (*callerFunc)(Register::Context&), unsigned int level) {
        registerList[level].push_back({ file, name, callerFunc });
    }
    ~Register() {
        for(unsigned int i = 0; i < 3; i++) {
            for (const signedFuncWrapper& item : registerList[1]) {
                (*item.callerFunc)(Context{ testData });
            }
        }
    }
    DataSet* testData;
private:
    typedef struct {
        const char* file;
        const char* name;
        void (*callerFunc)(Register::Context&);
    } signedFuncWrapper;
    vector<signedFuncWrapper> registerList[3];
};

Register globalRegister("", "");

class Registing {
public:
    Registing(Register reg,
        const char* file, const char* name,
        void (*callerFunc)(Register::Context&), unsigned int level) {
        reg.Add(file, name, callerFunc, level);
    }
};

/* ========== Testing ========== */

class Testing {
    public:
        Testing(const char* file, const char* name) {
            reg = Register(file, name);
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
        template<typename T> void Log(const char* file, int line, const char* varName, T value) {
            reg.testData->Add(new DataLog<T>(file, line, varName, value));
        }
        template<typename T> void Addition(const char * kind, const char* varname, T value) {
            cout << "      + " << kind << ": " << varname << " = " << value << endl;
        }
        DataSet* GetData() {
            return reg.testData;
        }
        ~Testing() {
            reg.testData->End(failed, clock() - start, failureCount);
        }
    private:
        clock_t start; // No need to report.
        unsigned int failureCount;
        bool failed;
        Register reg;
};

#define TEST(name) \
    void name(lightest::Testing& testing); \
    void call_ ## name(lightest::Register::Context& ctx){ \
        lightest::Testing testing(__FILE__, #name); \
        name(testing); \
        ctx.testData->Add(testing.GetData()); \
    } \
    lightest::Registing registing_ ## name(lightest::globalRegister, __FILE__, #name, call_ ## name, 1); \
    void name(lightest::Testing& testing)

} /* namespace ending */

/* ========== Default main functions ========== */

#define SIMPLER() lightest::moreOutput = false;
#define NO_COLOR() lightest::OutputColor = false;
#define FILTER(level) filtLevel = level;

#define MAIN int main() { return 0; }

#define LESS_MAIN \
    int main() { \
        SIMPLER(); FILTER(lightest::MSG_LOWER); \
        return 0; }

/* ========== Logging Macros ========== */

#define MSG(str) testing.Msg(__FILE__, __LINE__,(str))
#define WARN(str) testing.Warn(__FILE__, __LINE__,(str))
#define ERR(str) testing.Err(__FILE__, __LINE__, (str))
#define FAIL(str) testing.Fail(__FILE__, __LINE__, (str))
#define LOG(varName) testing.Log(__FILE__, __LINE__, #varName, (varName))

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