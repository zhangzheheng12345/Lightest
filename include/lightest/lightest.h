/*
| Lightest |
This is the core file of this library, which provides a lightest C++ unit
test framework. Licensed under MIT.
 - Github Repo: https://github.com/zhangzheheng12345/Lightest
 - Author's Github: https://github.com/zhangzheheng12345
*/

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
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

// For coloring on Windows
#ifdef _WIN_
#include <Windows.h>
#endif

namespace lightest {

using namespace std;

typedef const char* cstr;

/* ========== Output Color ==========*/

enum class Color { Reset = 0, Red = 41, Green = 42, Yellow = 43, Blue = 44 };
bool outputColor = true;  // Use NO_COLOR() to set false

void SetColor(Color color) {
#ifdef _WIN_
  // Set output mode to handle virtual terminal sequences
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) {
    return;
  }
  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) {
    return;
  }
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(hOut, dwMode)) {
    return;
  }
#endif
  if (outputColor) {
    cout << "\033[" << int(color) << "m";
  }
}

/* ========== Data ========== */

bool toOutput = true;              // Use NO_OUTPUT() to set to false
bool failedReturnNoneZero = true;  // Use RETURN_ZERO() to set to false

enum DataType { DATA_SET, DATA_REQ, DATA_UNCAUGHT_ERROR };

// Unitlity for transfering clock_t to ms,
// for on Linux clock_t's unit is us, while on Windows it's ms
inline double TimeToMs(clock_t time) { return double(time) / CLOCKS_PER_SEC * 1000; }
template <typename T>
const string AnyToString(const T value) {
  stringstream ss;
  ss << value;
  return ss.str();
}

// Untility for print a string with colorful background
// e.g. PRINT_LABEL(lightest::Color::Red, 1 << " failed test");
#define PRINT_LABEL(color, label)               \
  do {                                          \
    lightest::SetColor(color);                  \
    std::cout << label;                         \
    lightest::SetColor(lightest::Color::Reset); \
  } while (0)

// All test data classes should extend from Data
class Data {
 public:
  // For outputting
  virtual void Print() const = 0;
  void SetTabs(unsigned int tabs) { this->tabs = tabs; }
  unsigned int GetTabs() const { return tabs; }
  ostream& PrintTabs() const {
    // 3 spaces a group, for ''\t' may be too wide on some platforms
    // Must this->tabs - 1 first,
    // for this->tabs of global tests will be set to
    // 1
    for (unsigned int tabs = this->tabs - 1; tabs > 0; tabs--) cout << "   ";
    return cout;
  }
  // Offer type to enable transfer Data to exact class of test data
  virtual DataType Type() const = 0;
  virtual const bool GetFailed() const = 0;
  virtual ~Data() {}

 private:
  unsigned int tabs;
};

// Contain test actions on current level and sub tests' data
// Recursively call Print() to output
class DataSet : public Data {
 public:
  DataSet(cstr name_) : failed(false), duration(0), name(name_) {}
  void Add(Data* son) {
    son->SetTabs(GetTabs() + 1);
    if (son->GetFailed()) failed = true;
    sons.push_back(son);
  }
  void End(clock_t duration) { this->duration = duration; }
  void PrintSons() const {
    for (const Data* item : sons) {
      item->Print();
    }
  }
  void Print() const {
    PrintTabs();
    PRINT_LABEL(Color::Blue, " BEGIN ");
    cout << " " << name << endl;
    PrintSons();
    PrintTabs();
    if (failed) {
      PRINT_LABEL(Color::Red, " FAIL  ");
    } else {
      PRINT_LABEL(Color::Green, " PASS  ");
    }
    cout << " " << name << " " << TimeToMs(duration) << " ms" << endl;
  }
  DataType Type() const { return DATA_SET; }
  const bool GetFailed() const { return failed; }
  clock_t GetDuration() const { return duration; }
  cstr GetName() const { return name; }
  unsigned int GetSonsNum() const { return sons.size(); }
  // Receive a callback to iterate test actions and sub tests' data
  template <typename... restArgs>
  void IterSons(function<void(const Data*, restArgs...)> func, restArgs... args) const {
    for (const Data* item : sons) {
      func(item, args...);
    }
  }
  void IterSons(function<void(const Data*)> func) const {
    for (const Data* item : sons) {
      func(item);
    }
  }
  ~DataSet() {
    for (const Data* item : sons) {
      delete item;
    }
  }

 private:
  bool failed;
  clock_t duration;
  // Data of test actions and sub tests
  vector<const Data*> sons;
  cstr name;
};

// Data classes for test actions should to extend from DataUnit,
// for loggings should contain file & line information
class DataUnit {
 public:
  DataUnit(cstr file_, unsigned int line_) : file(file_), line(line_) {}
  virtual ~DataUnit() {}
  unsigned int GetLine() const { return line; }
  cstr GetFileName() const { return file; }

 protected:
  cstr file;
  unsigned int line;
};

// Data class of REQ assertions
class DataReq : public Data, public DataUnit {
 public:
  DataReq(cstr file_, unsigned int line_, const string actual_, const string expected_,
          cstr aType, cstr eType, cstr operator__, cstr expr_, bool failed_)
      : DataUnit(file_, line_),
        actual(actual_),
        expected(expected_),
        actualType(aType),
        expectedType(eType),
        operator_(operator__),
        expr(expr_),
        failed(failed_) {}
  // Print data of REQ if assertion fails
  void Print() const {
    if (failed) {
      PrintTabs();
      PRINT_LABEL(Color::Red, " FAIL  ");
      cout << " " << file << ":" << line << ":"
           << " REQ [" << expr << "] failed" << endl;
      PrintTabs() << "    ├───── ACTUAL: " << string(string(operator_).size(), ' ') << " "
                  << actual << endl;
      PrintTabs() << "    └─── EXPECTED: " << operator_ << " " << expected << endl;
    }
  }
  DataType Type() const { return DATA_REQ; }
  const string& GetActual() const { return actual; }
  const string& GetExpected() const { return expected; }
  cstr GetActualType() const { return actualType; }
  cstr GetExpectedType() const { return expectedType; }
  cstr GetOperator() const { return operator_; }
  cstr GetExpr() const { return expr; }
  const bool GetFailed() const { return failed; }

 private:
  const string actual, expected;
  cstr actualType, expectedType, operator_, expr;
  const bool failed;
};

class DataUncaughtError : public Data, public DataUnit {
 public:
  DataUncaughtError(cstr file_, unsigned int line_, cstr errorMsg_)
      : DataUnit(file_, line_), errorMsg(errorMsg_) {}
  void Print() const {
    PrintTabs();
    PRINT_LABEL(Color::Red, " ERROR ");
    cout << " " << file << ":" << line << ": Uncaught error [" << errorMsg << "]" << endl;
  }
  DataType Type() const { return DATA_UNCAUGHT_ERROR; }
  const bool GetFailed() const { return true; }

 private:
  cstr errorMsg;
};

/* ========== Register ========== */

class Register {
 public:
  Register(cstr name) { testData = new DataSet(name); }
  Register() { testData = NULL; }
  typedef struct {
    DataSet* testData;
    int argn;
    char** argc;
  } Context;
  // Register a CONFIG, TEST, or DATA
  void Add(cstr name, function<void(Context&)> callerFunc) {
    registerList.push_back({name, callerFunc});
  }
  void RunRegistered() {
    Context ctx = Context{testData, argn, argc};
    for (const signedFuncWrapper& item : registerList) {
      item.callerFunc(ctx);
    }
  }
  // Restore argn & argc for CONFIG
  static void SetArg(int argn, char** argc) { Register::argn = argn, Register::argc = argc; }
  DataSet* testData;

 private:
  typedef struct {
    cstr name;
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
  Registering(Register& reg, cstr name, void (*callerFunc)(Register::Context&)) {
    reg.Add(name, callerFunc);
  }
};

/* ========== Testing ========== */

// An instance of Testing is for adding test data and adding sub tests
class Testing {
 public:
  // level_: 1 => global tests, 2 => sub tests, 3 => sub sub tests ...
  Testing(cstr name, unsigned int level_)
      : level(level_), start(clock()), failed(false), reg(name) {
    reg.testData->SetTabs(level);  // Give correct tabs to its sons
  }
  void AddSub(cstr name, function<void(Register::Context&)> callerFunc) {
    reg.Add(name, callerFunc);
  }
  // To add children data
  void AddData(Data* data) { reg.testData->Add(data); }
  DataSet* GetData() const { return reg.testData; }
  unsigned int GetLevel() const { return level; }
  void End() {
    reg.RunRegistered();  // Run sub tests
    reg.testData->End(clock() - start);
  }

 private:
  const unsigned int level;
  const clock_t start;  // No need to report.
  bool failed;
  Register reg;
};

};  // namespace lightest

/* ========== Registering Macros ========== */

// Avoid undef or redefine buildin macro warning
// Undef and then define again to have prettier file name
#ifndef TEST_FILE_NAME
#ifdef __FILE_NAME__
#define TEST_FILE_NAME __FILE_NAME__
#else
#define TEST_FILE_NAME __FILE__
#endif
#endif

// Untility for catching an error and return its detail as type const char*
#define CATCH(sentence)                   \
  ([&]() -> const char* {                 \
    try {                                 \
      sentence;                           \
    } catch (const char* str) {           \
      return str;                         \
    } catch (const std::exception& err) { \
      return err.what();                  \
    } catch (...) {                       \
      return "Unknown type error";        \
    }                                     \
    return nullptr;                       \
  })()

// To define user's configuarations
// Pre-define argn and argc for user's configurations
#define CONFIG(name)                                                               \
  void name(int argn, char** argc);                                                \
  void call_##name(lightest::Register::Context& ctx) { name(ctx.argn, ctx.argc); } \
  lightest::Registering registering_##name(lightest::globalRegisterConfig, #name,  \
                                           call_##name);                           \
  void name(int argn, char** argc)

// To define a test
#define TEST(name)                                                                            \
  void name(lightest::Testing& testing);                                                      \
  void call_##name(lightest::Register::Context& ctx) {                                        \
    lightest::Testing testing(#name, 1);                                                      \
    const char* errorMsg = CATCH(name(testing));                                              \
    if (errorMsg)                                                                             \
      testing.AddData(new lightest::DataUncaughtError(TEST_FILE_NAME, __LINE__, errorMsg));   \
    testing.End();                                                                            \
    ctx.testData->Add(testing.GetData()); /* Colletct data */                                 \
  }                                                                                           \
  lightest::Registering registering_##name(lightest::globalRegisterTest, #name, call_##name); \
  void name(lightest::Testing& testing)

// To define a test data processor
// Pre-define data to provide a readonly object containing all the test data
#define DATA(name)                                                                            \
  void name(const lightest::DataSet* data);                                                   \
  void call_##name(lightest::Register::Context& ctx) { name(ctx.testData); }                  \
  lightest::Registering registering_##name(lightest::globalRegisterData, #name, call_##name); \
  void name(const lightest::DataSet* data)

#define SUB(name)                                                                   \
  static std::function<void(lightest::Testing&)> name;                              \
  std::function<void(lightest::Register::Context&)> call_##name =                   \
      [&testing](lightest::Register::Context& ctx) {                                \
        lightest::Testing testing_(#name, testing.GetLevel() + 1);                  \
        const char* errorMsg = CATCH(name(testing_));                               \
        if (errorMsg)                                                               \
          testing_.AddData(                                                         \
              new lightest::DataUncaughtError(TEST_FILE_NAME, __LINE__, errorMsg)); \
        testing_.End();                                                             \
        ctx.testData->Add(testing_.GetData());                                      \
      };                                                                            \
  testing.AddSub(#name, call_##name);                                               \
  name = [=](lightest::Testing & testing)

/* ========== Configuration Macros ========== */

#define NO_COLOR() lightest::outputColor = false;
#define NO_OUTPUT() lightest::toOutput = false;
#define RETURN_ZERO() lightest::failedReturnNoneZero = false;

/* ========== Main ========== */

int main(int argn, char* argc[]) {
  using namespace lightest;
  using namespace std;
  // Offer arn & argc for CONFIG
  Register::SetArg(argn, argc);
  // Only test registerer need this, for test data will only be added in test
  // process
  globalRegisterTest.testData->SetTabs(0);
  // 1. Run CONFIG
  // 2. Run tests (TEST)
  // 3. Pass test data to DATA registerer
  // 4. Run DATA
  globalRegisterConfig.RunRegistered();
  globalRegisterTest.RunRegistered();
  globalRegisterData.testData = globalRegisterTest.testData;
  // Optionally print the default outputs
  if (toOutput) {
    globalRegisterData.testData->PrintSons();
  }
  globalRegisterData.RunRegistered();
  if (globalRegisterData.testData->GetFailed())
    PRINT_LABEL(Color::Red, " ✕ FAILED ✕ ");
  else
    PRINT_LABEL(Color::Green, " ✓ SUCCEEDED ✓ ");
  PRINT_LABEL(Color::Blue, " " << TimeToMs(clock()) << " ms used ");
  cout << endl << endl;
  return globalRegisterData.testData->GetFailed() && failedReturnNoneZero;
}

/* ========= Timer Macros =========== */

// Unit: minisecond (ms)

// Run once and messure the time
#define TIMER(sentence)                         \
  ([&]() -> double {                            \
    clock_t start = clock();                    \
    (sentence);                                 \
    return lightest::TimeToMs(clock() - start); \
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
    return lightest::TimeToMs(sum) / times;                 \
  }())

/* ========== Assertion Macros ========== */

// REQ assertion
// Additionally return a bool: true => pass, false => fail
#define REQ(actual, operator, expected)                                                     \
  ([&]() -> bool {                                                                          \
    auto actual_ = (actual);                                                                \
    auto expected_ = (expected);                                                            \
    bool res = (actual_) operator(expected_);                                               \
    testing.AddData(new lightest::DataReq(                                                  \
        TEST_FILE_NAME, __LINE__, lightest::AnyToString(actual_),                           \
        lightest::AnyToString(expected_), typeid(actual_).name(), typeid(expected_).name(), \
        #operator, #actual " " #operator" " #expected, !res));                              \
    return res;                                                                             \
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
