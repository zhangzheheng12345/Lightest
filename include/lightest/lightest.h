/********************
| Lightest |
This is the core file of this library, which provides a lightest C++ unit test
framework. MIT licensed. Github Repo:
https://github.com/zhangzheheng12345/Lightest Author's Github:
https://github.com/zhangzheheng12345
********************/

#ifndef _LIGHTEST_H_
#define _LIGHTEST_H_

// Deal with different OS
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
#include <functional>
#include <iostream>
#include <vector>

// For coloring on Windows
#ifdef _WIN_
#include <Windows.h>
#endif

namespace lightest {
using namespace std;

/* ========== Output Color ==========*/

enum class Color { Reset = 0, Red = 41, Green = 42, Yellow = 43, Blue = 44 };
bool OutputColor = true;  // Use NO_COLOR() to set false

void SetColor(Color color) {
  if (OutputColor) {
#if defined(_LINUX_) || \
    defined(_MAC_)  // Use ASCII color code on Linux and MacOS
    cout << "\033[" << int(color) << "m";
#elif defined(_WIN_)  // Use Windows console API on Windows
    unsigned int winColor;
    switch (color) {
      case Color::Reset:
        winColor = 0x07;
        break;
      case Color::Red:
        winColor = 0xc0;
        break;
      case Color::Green:
        winColor = 0xa0;
        break;
      case Color::Yellow:
        winColor = 0xe0;
        break;
      case Color::Blue:
        winColor = 0x90;
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

bool toOutput = true;  // Use NO_OUTPUT() to set to false

enum DataType { DATA_SET, DATA_REQ };

// All test data classes should extend from Data
class Data {
 public:
  // For outputting
  virtual void Print() const = 0;
  void SetTabs(unsigned int tabs) { this->tabs = tabs; }
  unsigned int GetTabs() { return tabs; }
  ostream& PrintTabs() const {
    // Must be 4 spaces a group, for \t may be too wide on some platforms
    // Must this->tabs - 1 first,
    // for this->tabs of tests added to the gloabl registerer will be set to
    // 1
    for (unsigned int tabs = this->tabs - 1; tabs > 0; tabs--) cout << "  ";
    return cout;
  }
  // Offer type to enable transfer Data to exact class of test data
  virtual DataType Type() const = 0;
  virtual ~Data() {}

 private:
  unsigned int tabs;
};

// Contain test actions on current level and sub tests' data
// Recursively call Print() to give out all the outpus
class DataSet : public Data {
 public:
  DataSet(const char* name) { this->name = name, duration = 0; }
  void Add(Data* son) {
    son->SetTabs(GetTabs() + 1);
    sons.push_back(son);
  }
  void End(bool failed, clock_t duration) {
    this->failed = failed, this->duration = duration;
  }
  void PrintSons() const {
    for (const Data* item : sons) {
      item->Print();
    }
  }
  void Print() const {
    PrintTabs();
    SetColor(Color::Blue);
    cout << " BEGIN ";
    SetColor(Color::Reset);
    cout << " " << name << endl;
    PrintSons();
    PrintTabs();
    if (failed) {
      SetColor(Color::Red);
      cout << " FAIL  ";
    } else {
      SetColor(Color::Green);
      cout << " PASS  ";
    }
    SetColor(Color::Reset);
    cout << " " << name << " " << double(duration) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
  }
  DataType Type() const { return DATA_SET; }
  bool GetFailed() const { return failed; }
  clock_t GetDuration() const { return duration; }
  const char* GetName() const { return name; }
  // Should offer a callback to iterate test actions and sub tests' data
  void IterSons(void (*func)(const Data*)) const {
    for (const Data* item : sons) {
      func(item);
    }
  }

 private:
  bool failed;
  clock_t duration;
  vector<const Data*> sons;
  const char* name;
};

// Data classes for test actions should to extend from DataUnit
// Because loggings should contain file & line information
class DataUnit {
 public:
  unsigned int GetLine() const { return line; }
  const char* GetFileName() const { return file; }

 protected:
  unsigned int line;
  const char* file;
};

// Data class of REQ assertions
template <class T, class U>  // Different types for e.g. <int> == <double>
class DataReq : public Data, public DataUnit {
 public:
  DataReq(const char* file, unsigned int line, const T& actual_,
          const T& expected_, const char* operator_, const char* expr,
          bool failed_)
      : actual(actual_), expected(expected_), failed(failed_) {
    this->file = file, this->line = line, this->operator_ = operator_,
    this->expr = expr;
  }
  // Print data of REQ if assertion fails
  void Print() const {
    if (failed) {
      PrintTabs();
      SetColor(Color::Red);
      cout << " FAIL ";
      SetColor(Color::Reset);
      cout << " " << file << ":" << line << ":"
           << " REQ [" << expr << "] failed" << endl;
      PrintTabs() << "    +   ACTUAL: " << actual << endl;
      PrintTabs() << "    + EXPECTED: " << operator_ << " " << expected << endl;
    }
  }
  DataType Type() const { return DATA_REQ; }
  const T GetActual() const { return actual; }
  const U GetExpected() const { return GetExpected; }
  const char* GetOperator() const { return operator_; }
  const char* GetExpr() const { return expr; }
  const bool GetFailed() const { return failed; }

 private:
  const T actual;
  const U expected;
  const char *operator_, *expr;
  const bool failed;
};

/* ========== Register ========== */

class Register {
 public:
  Register(const char* name) { testData = new DataSet(name); }
  Register() { testData = NULL; }
  typedef struct {
    DataSet* testData;
    int argn;
    char** argc;
  } Context;
  void Add(const char* name, function<void(Context&)> callerFunc) {
    registerList.push_back({name, callerFunc});
  }
  // Run tests registered
  void RunRegistered() {
    Context ctx = Context{testData, argn, argc};
    for (const signedFuncWrapper& item : registerList) {
      item.callerFunc(ctx);
    }
  }
  // Restore argn & argc for CONFIG
  static void SetArg(int argn, char** argc) {
    Register::argn = argn, Register::argc = argc;
  }
  DataSet* testData;

 private:
  typedef struct {
    const char* name;
    function<void(Register::Context&)> callerFunc;
  } signedFuncWrapper;
  vector<signedFuncWrapper> registerList;
  static int argn;
  static char** argc;
};
int Register::argn = 0;
char** Register::argc = nullptr;

Register globalRegisterConfig("");
Register globalRegisterTest("");
Register globalRegisterData("");

class Registering {
 public:
  Registering(Register& reg, const char* name,
              void (*callerFunc)(Register::Context&)) {
    reg.Add(name, callerFunc);
  }
};

/* ========== Testing ========== */

class Testing {
 public:
  // level_: 1 => global tests, 2 => sub tests, 3 => sub sub tests ...
  Testing(const char* name, unsigned int level_)
      : start(clock()), level(level_) {
    reg = Register(name);
    reg.testData->SetTabs(level);  // Give correct tabs to its sons
    failed = false;
  }
  // Add a test data unit of a REQ assertion
  template <typename T, typename U>  // Differnt type for e.g. <int> == <double>
  void Req(const char* file, int line, const T& actual, const U& expected,
           const char* operator_, const char* expr, bool failed) {
    reg.testData->Add(new DataReq<T, U>(file, line, actual, expected, operator_,
                                        expr, failed));
    this->failed = failed;
  }
  void AddSub(const char* name, function<void(Register::Context&)> callerFunc) {
    reg.Add(name, callerFunc);
  }
  DataSet* GetData() const { return reg.testData; }
  unsigned int GetLevel() const { return level; }
  ~Testing() {
    reg.RunRegistered();  // Run sub tests
    reg.testData->End(failed, clock() - start);
  }

 private:
  const unsigned int level;
  const clock_t start;  // No need to report.
  bool failed;
  Register reg;
};

}  // namespace lightest

/* ========== Registing Macros ========== */

#define CONFIG(name)                                                       \
  void name(int argn, char** argc);                                        \
  void call_##name(lightest::Register::Context& ctx) {                     \
    name(ctx.argn, ctx.argc);                                              \
  }                                                                        \
  lightest::Registering registering_##name(lightest::globalRegisterConfig, \
                                           #name, call_##name);            \
  void name(int argn, char** argc)

#define TEST(name)                                                       \
  void name(lightest::Testing& testing);                                 \
  void call_##name(lightest::Register::Context& ctx) {                   \
    lightest::Testing testing(#name, 1);                                 \
    name(testing);                                                       \
    ctx.testData->Add(testing.GetData());                                \
  }                                                                      \
  lightest::Registering registering_##name(lightest::globalRegisterTest, \
                                           #name, call_##name);          \
  void name(lightest::Testing& testing)

#define DATA(name)                                                           \
  void name(const lightest::DataSet* data);                                  \
  void call_##name(lightest::Register::Context& ctx) { name(ctx.testData); } \
  lightest::Registering registering_##name(lightest::globalRegisterData,     \
                                           #name, call_##name);              \
  void name(const lightest::DataSet* data)

#define SUB(name)                                                  \
  static std::function<void(lightest::Testing&)> name;             \
  std::function<void(lightest::Register::Context&)> call_##name =  \
      [&testing](lightest::Register::Context& ctx) {               \
        lightest::Testing testing_(#name, testing.GetLevel() + 1); \
        name(testing_);                                            \
        ctx.testData->Add(testing_.GetData());                     \
      };                                                           \
  testing.AddSub(#name, call_##name);                              \
  name = [=](lightest::Testing & testing)

/* ========== Configuration Macros ========== */

#define NO_COLOR() lightest::OutputColor = false;
#define NO_OUTPUT() lightest::toOutput = false;

/* ========== Main ========== */

int main(int argn, char* argc[]) {
  // Offer arn & argc for CONFIG
  lightest::Register::SetArg(argn, argc);
  // Only test registerer need this, for test data will only be added in test
  // process
  lightest::globalRegisterTest.testData->SetTabs(0);
  // 1. Run CONFIG
  // 2. Run tests (TEST)
  // 3. Pass test data to DATA registerer
  // 4. Run DATA
  lightest::globalRegisterConfig.RunRegistered();
  lightest::globalRegisterTest.RunRegistered();
  lightest::globalRegisterData.testData = lightest::globalRegisterTest.testData;
  lightest::globalRegisterData.RunRegistered();
  // Optionally print the default outputs
  if (lightest::toOutput) {
    lightest::globalRegisterData.testData->PrintSons();
  }
  std::cout << "Done. " << double(clock()) / CLOCKS_PER_SEC * 1000 << "ms used."
            << std::endl;
  return 0;
}

/* ========= Timer Macros =========== */

// Unit: minisecond (ms)

// Run once and messure the time
#define TIMER(sentence)                                     \
  ([&]() -> double {                                        \
    clock_t start = clock();                                \
    (sentence);                                             \
    return double(clock() - start) / CLOCKS_PER_SEC * 1000; \
  }())

// Run several times and return the average time
#define AVG_TIMER(sentence, times)                          \
  ([&]() -> double {                                        \
    clock_t sum = 0, start;                                 \
    for (unsigned int index = 1; index <= times; index++) { \
      start = clock();                                      \
      (sentence);                                           \
      sum += clock() - start;                               \
    }                                                       \
    return double(sum) / times / CLOCKS_PER_SEC * 1000;     \
  }())

/* ========== Assertion Macros ========== */

// REQ assertion
// Additionally return a bool: true => pass, false => fail
#define REQ(actual, operator, expected)                          \
  ([&]() -> bool {                                               \
    bool res = (actual) operator(expected);                      \
    testing.Req(__FILE__, __LINE__, actual, expected, #operator, \
                #actual " " #operator" " #expected, !res);       \
    return res;                                                  \
  })()

// Condition must be true or stop currnet test
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