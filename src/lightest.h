/********************
| Lightest |
This is the core file of this library, which provides a lightest C++ unit test
framework. MIT licensed. Github Repo:
https://github.com/zhangzheheng12345/Lightest Author's Github:
https://github.com/zhangzheheng12345
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

#include <ctime>
#include <iostream>
#include <vector>

#ifdef _WIN_
#include <Windows.h>
#endif

namespace lightest {
using namespace std;

/* ========== Output Color ==========*/

// TODO: use enum class
enum Color { Reset = 0, Black = 30, Red = 31, Green = 32, Yellow = 33 };
bool OutputColor = true;

void SetColor(Color color) {
  if (OutputColor) {
#if defined(_LINUX_) || defined(_MAC_)
    cout << "\033[" << color << "m";
#elif defined(_WIN_)
    unsigned int winColor;
    switch (color) {
      case Reset:
        winColor = 0x07;
        break;
      case Black:
        winColor = 0x00;
      case Red:
        winColor = 0x0c;
        break;
      case Green:
        winColor = 0x0a;
        break;
      case Yellow:
        winColor = 0x0e;
        break;
      default:
        winColor = 0x07;
        break;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), winColor);
#endif
  }
}

/* ========== Data ========== */

bool toOutput = true;  // Use NO_OUTPUT to set to false

enum class DataType { DATA_SET, DATA_REQ };

class Data {
 public:
  virtual void Print() const = 0;
  virtual DataType Type() const = 0;
  virtual ~Data() {}
};

class DataSet : public Data {
 public:
  DataSet(const char* name) { this->name = name, duration = 0; }
  void Add(Data* son) { sons.push_back(son); }
  void End(bool failed, clock_t duration) {
    this->failed = failed, this->duration = duration;
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
    if (failed) {
      SetColor(Red);
      cout << " FAIL ";
    } else {
      SetColor(Green);
      cout << " PASS ";
    }
    SetColor(Reset);
    cout << duration / CLOCKS_PER_SEC * 1000 << "ms" << endl;
  }
  DataType Type() const { return DataType::DATA_SET; }
  bool GetFailed() const { return failed; }
  clock_t GetDuration() const { return duration; }
  void IterSons(void (*func)(const Data*)) const {
    for (const Data* item : sons) {
      func(item);
    }
  }
  const char* name;

 private:
  bool failed;
  clock_t duration;
  vector<Data*> sons;
};

class DataUnit {
 public:
  const char* file;
  unsigned int GetLine() const { return line; }

 protected:
  unsigned int line;
};

template <class T, class U>
class DataReq : public Data, public DataUnit {
 public:
  DataReq(const char* file, unsigned int line, const T& actual_,
          const T& expected_, const char* operator_, const char* expr,
          bool failed_)
      : actual(actual_), expected(expected_), failed(failed_) {
    this->file = file, this->line = line, this->operator_ = operator_,
    this->expr = expr;
  }
  void Print() const {
    if (failed) {
      SetColor(Red);
      cout << "    [Fail ] ";
      SetColor(Reset);
      cout << file << ":" << line << ":"
           << " REQ [" << expr << "] failed" << endl;
      cout << "        + ACTUAL: " << actual << endl;
      cout << "        + EXPECTED: " << operator_ << " " << expected << endl;
    }
  }
  DataType Type() const { return DataType::DATA_REQ; }
  const T actual;
  const U expected;
  const char *operator_, *expr;
  const bool failed;
};

/* ========== Signer ========== */

class Register {
 public:
  Register(const char* name) {
    testData = new DataSet(name);
    for (unsigned int i = 0; i < 3; i++)
      registerList[i] = vector<signedFuncWrapper>(0);
  }
  Register() {
    testData = NULL;
    for (unsigned int i = 0; i < 3; i++)
      registerList[i] = vector<signedFuncWrapper>(0);
  }
  typedef struct {
    DataSet* testData;
    int argn;
    char** argc;
  } Context;
  void Add(const char* name, void (*callerFunc)(Context&), unsigned int level) {
    registerList[level].push_back({name, callerFunc});
  }
  void RunRegistered() {
    Context ctx = Context{testData, argn, argc};
    for (unsigned int i = 0; i < 3; i++) {
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
  Registering(Register& reg, const char* name,
              void (*callerFunc)(Register::Context&), unsigned int level) {
    reg.Add(name, callerFunc, level);
  }
};

/* ========== Testing ========== */

class Testing {
 public:
  Testing(const char* name) {
    reg = Register(name);
    failed = false;
    start = clock();
  }
  template <typename T, typename U>
  void Req(const char* file, int line, const T& actual, const U& expected,
           const char* operator_, const char* expr, bool failed) {
    reg.testData->Add(new DataReq<T, U>(file, line, actual, expected, operator_,
                                        expr, failed));
    this->failed = failed;
  }
  DataSet* GetData() { return reg.testData; }
  ~Testing() {
    reg.RunRegistered();
    reg.testData->End(failed, clock() - start);
  }

 private:
  clock_t start;  // No need to report.
  bool failed;
  Register reg;
};

}  // namespace lightest

/* ========== Registing macros ========== */

#define CONFIG(name)                                                        \
  void name(int argn, char** argc);                                         \
  void call_##name(lightest::Register::Context& ctx) {                      \
    name(ctx.argn, ctx.argc);                                               \
  }                                                                         \
  lightest::Registering registering_##name(lightest::globalRegister, #name, \
                                           call_##name, 0);                 \
  void name(int argn, char** argc)
#define TEST(name)                                                          \
  void name(lightest::Testing& testing);                                    \
  void call_##name(lightest::Register::Context& ctx) {                      \
    lightest::Testing testing(#name);                                       \
    name(testing);                                                          \
    ctx.testData->Add(testing.GetData());                                   \
  }                                                                         \
  lightest::Registering registering_##name(lightest::globalRegister, #name, \
                                           call_##name, 1);                 \
  void name(lightest::Testing& testing)
#define DATA(name)                                                           \
  void name(const lightest::DataSet* data);                                  \
  void call_##name(lightest::Register::Context& ctx) { name(ctx.testData); } \
  lightest::Registering registering_##name(lightest::globalRegister, #name,  \
                                           call_##name, 2);                  \
  void name(const lightest::DataSet* data)

/* ========== Configuration macros ========== */

#define NO_COLOR() lightest::OutputColor = false;
#define FILTER(level) lightest::filtLevel = level;
#define NO_OUTPUT() lightest::toOutput = false;

/* ========== Main ========== */

int main(int argn, char* argc[]) {
  lightest::Register::SetArg(argn, argc);
  lightest::globalRegister.RunRegistered();
  if (lightest::toOutput) {
    lightest::globalRegister.testData->PrintSons();
  }
  std::cout << "Done. " << clock() / CLOCKS_PER_SEC * 1000 << "ms used."
            << std::endl;
  return 0;
}

/* ========= Timer Macros =========== */

// unit: minisecond(ms)
#define TIMER(sentence)                               \
  ([&]() {                                            \
    clock_t start = clock();                          \
    (sentence);                                       \
    return (clock() - start) / CLOCKS_PER_SEC * 1000; \
  }())
#define AVG_TIMER(sentence, times)                          \
  ([&]() {                                                  \
    clock_t sum = 0, start;                                 \
    for (unsigned int index = 1; index <= times; index++) { \
      start = clock();                                      \
      (sentence);                                           \
      sum += clock() - start;                               \
    }                                                       \
    return double(sum) / times / CLOCKS_PER_SEC * 1000;     \
  }())

/* ========== Assertion Macros ========== */

// true => pass, false => fail
#define REQ(actual, operator, expected)                          \
  ([&]() -> bool {                                               \
    bool res = (actual) operator(expected);                      \
    testing.Req(__FILE__, __LINE__, actual, expected, #operator, \
                #actual " " #operator" " #expected, !res);       \
    return res;                                                  \
  })()

// condition must be true or stop currnet test
#define MUST(condition) \
  do {                  \
    if (!(condition)) { \
      return;           \
    }                   \
  } while (0)

#undef _LINUX_
#undef _WIN_
#undef _MAC_

#endif