![Lightest](/imgs/logo.png)

<h1 align="center">Lightest</h1>

![Basic Tests Status](https://github.com/zhangzheheng12345/Lightest/actions/workflows/basic_tests.yml/badge.svg?branch=main)
![Benchmark Tests Status](https://github.com/zhangzheheng12345/Lightest/actions/workflows/benchmark_tests.yml/badge.svg?branch=main)
![MIT Licensed](https://img.shields.io/badge/license-MIT-blue.svg)

An extremely light but meanwhile strong C++ unit test framework. You should read its name as Ligh-test, with `-test` stressed. It's headed-only.

The core library file is `/include/lightest/lightest.h`, a very light yet strong core. There are optional extensions provided to make **Lightest** much more powerful.

Following features are supported:

* Automatic registration for testing, configuring and data analyzing functions.
* Assertions.
* Pretty output, include coloring (cross platform supported).
* Test data collecting.
* Timers.
* User configurations are supported if you want.
* Beautiful data analysis extension.
* Command line arguments extension.

Your compiler has to support C++11. Additionally, the project has only been tested on clang++ on Ubuntu, and MSVC (Visual Studio) on Windows 10.

## Why Lightest

Very light, header-only, yet strong, flexible & customizable.

Basic features for a unit test framework are all provided in the core, enabling you to write beautiful and pithy tests, and there are pre-provided extensions to offer further feature. Write your own extensions is easy as well. These are especially convenient for small and light projects.

Besides, **Lightest** is also extremely fast powered by its lightweight. I did a benchmark test (details under `benchmark/`), in which 1000 simple tests each with one passing assertion are provided for both **Lightest** and GTest. On one of my machine, for instance, GTest used around 600 ms to run all the tests, while for **Lightest**, it only took around 15 ms. Amazing, isn't it?

## A quick look

A short piece of test using Lightest:

```C++
#include <lightest/lightest.h>

TEST(Test) {
  REQ(1, ==, 1); // Pass
  REQ(1, ==, 2); // Oops! Fail
  int a = 1, b = 1, c = 2;
  REQ(a, ==, b); // Pass
  REQ(a, ==, c); // Oops! Fail again
  SUB(SubTest) {
    SUB(SubSubTest) {
      REQ(a, ==, b); // Pass
    };
  };
}
```

Outputs simple and tidy:

```
 BEGIN  Test
    FAIL  test.cpp:5: REQ [1 == 2] failed
       ├───── ACTUAL:    1
       └─── EXPECTED: == 2
    FAIL  test.cpp:8: REQ [a == c] failed
       ├───── ACTUAL:    1
       └─── EXPECTED: == 2
    BEGIN  SubTest
       BEGIN  SubSubTest
       PASS   SubSubTest 0.001 ms
    PASS   SubTest 0.003 ms
 FAIL   Test 0.109 ms
 ✕ FAILED ✕  1.518 ms used
```

Outputs should be quite pretty with coloring if your platform is Windows, Linux or Mac.

## Usage

You just need to add files under `include/lightest/` to your project in any form you like and then include it in your test files. If you just need the core and don't need the extensions, then just add `include/lightest/lightest.h`. You can use any build system, and here we provide a suggested way to integrate **Lightest** with CMake & CTest.

(**Caution:** You must compile different test files into different executable files instead of linking them into one. This is because there are definitions of global variables and functions in **Lightest** header files. Linking them will cause error.)

### Work with CMake & CTest

A suggested way:

1. Copy `include` (the whole directory) into `test/` of your project.
2. Create `test/test.cpp`.
3. Create `test/CMakeLists.txt` and add:

```CMake
# in test/CMakeLists.txt
cmake_minimum_required(VERSION 3.10) # Change the version if you need

project(ProjectTest) # Change the name as you like

# Add Lightest
add_library(lightest INTERFACE)
add_library(lightest::lightest ALIAS)
target_include_directories(lightest INTERFACE ${PROJECT_SOURCE_DIR}/include)

# No linking. Each test file -> one executable program
add_executable(${PROJECT_NAME} test.cpp)
target_link_libraries(${PROJECT_NAME} lightest::lightest)
add_test(${PROJECT_NAME} ${PROJECT_NAME})
```

4. Add following to the `CMakeLists.txt` file in the project's root directory:

```CMake
enable_testing()
add_subdirecty(${PROJECT_SOURCE_DIR}/test)
```

5. Use `#include <lightest/lightest.h>` in `test/test.cpp`.

### To add tests

Use macro `TEST(name)` to define a test named 'name'.

Here is an example showing how to add tests:

```C++
// in global scope
TEST(Test) {
  REQ(1, ==, 1); // Pass, of course
}
```

Use macro `SUB(name)` to add sub tests. Nesting is supported.

Example:

```C++
// inside global tests or sub tests
SUB(SubTest) {
  REQ(1, ==, 1); // Pass
  SUB(SubInSub) {
    REQ(1, ==, 1); // Pass
  }; // * semicolon required
}; // * semicolon required
```

All the defined tests will be automatically run because auto registration is supported.

Uncaught errors thrown out by tests will be caught and recorded, and the current test will be stopped and set to failed. Error details in form of `const char*` and `const std::exception&` can be obtained and included in test data, while other forms will be marked as `Unknown type error`.

**Caution** that variables outside `SUB` (except global variables) is immutable for code inside `SUB` (all `const`). Read-only capture is used in lambda partially because of life cycle problems and partially because shared variables aren't suggested to change. They should be fixed for all sub tests. For example, such code is unavailable:

```C++
// ! invalid code
TEST(TestChangeShared) {
  int shared = 0;
  SUB(SubTestChangeShared) {
    shared = 1; // compiling error: shared is `const`
    REQ(a, ==, 1);
  };
}
```

So to accomplish features like fixtures, you should define a class wrapping shared variables and get an instance in every sub test.

Example for fixture:

```C++
TEST(TestWithFixture) {
  class Fixture { // Define a fixture class in shared scope
   public:
    // Shared variables here
    int shared = 0;
  };
  SUB(SubTestFixture) {
    Fixture fixture; // Get an instance in every sub test
    fixture.shared = 1; // Now changeable
    REQ(fixture.shared, ==, 1); // Pass
  };
}
```

### Assertion macros

Use `REQ(actual, operator, expected)` to compare the actual value and the expected value. If the assertion fails, it'll output the actual value and the expected value. 

As for some situations like that you just want the returning value of a function to be true, pray write `REQ(func(), ==, true)` which is more readable and also fit **Lightest**.

We also provide `MUST(condition)`. The current test will be terminated if `condition` is false.

Example:

```C++
// in a test
int a = 1, b = 1, c = 2;
REQ(a, ==, b); // Pass with no output
REQ(a, ==, c); // Fail and output:
// REQ [1 == 2] failed
//    ├───── ACTUAL:    1
//    └─── EXPECTED: == 2
MUST(REQ(1, ==, 2)); // Fail and the current test will be terminated
```

### Timer macros

* `TIMER(sentence)` runs the sentence provided, and returns how long the sentence spends running. It returns type `clock_t`, ms.
* `AVG_TIMER(sentence, times)` runs the sentence `times` times, and then returns the average time.
* Always minisecond(ms) as the time unit.

Example:

```C++
TIMER(std::cout << "One Hello" << std::endl); // Return how long the sentence spends running
AVG_TIMER(std::cout << "Avg Hello" << std::endl, 1000); // Run it 1000 times and return the average time
```

### Configuration

You can write configurations like this (`CONFIG` functions are always run before `TEST`s):

```C++
// in global scope
CONFIG(Config) {
  NO_COLOR();
  // To get command line arguments
  // argn & argc pre-defined
  for(; argn > 0; argn--, argc++)
    std::cout << *argc << std::endl; // Output all args
}
```

* `NO_COLOR()` makes outputs get no coloring. Useful when you want to write outputs to a file.
* `NO_OUTPUT()` forbids the default outputting system to give out the loggings. Useful when you only want to deal the test data yourself and don't want any default output.
* `RETURN_ZERO()` makes main always returns 0. No returning 1 when there are failed tests.
* `argn` and `argc` are pre-defined in configuring functions.

Also an extension for converting command line arguments to **Lightest** configurations is provided. Simply include `lightest/arg_config_ext.h` and add `ARG_CONFIG();` to use it. Following arguments are supported:

* `--no-color` or `-nc` to disable coloring.
* `--no-output` or `-no` to disable default outputs.
* `--return-zero`, `--return-0` or `-r0` to disable returning 1 when failing.

### Data analysis

An extension for convenient data analysis and beautiful total report is provided. Just include `lightest/data_analysis_ext.h` to use it. Here is an example of using this extension:

```C++
#include <lightest/lightest.h>
#include <lightest/data_analysis_ext.h>

TEST(TestPass) { REQ(1, ==, 1); }
TEST(TestFail) {
  SUB(TestSubPass) { REQ(1, ==, 1); };
  SUB(TestSubFail) { REQ(1, ==, 2); };
}

REPORT() {
  // Currently provide these 3 options
  REPORT_FAILED_TESTS(); // List all failed test, sub tests outputted with tabs
  REPORT_PASS_RATE(); // Calculate the passing rate of global tests
  REPORT_AVG_TIME(); // Report average time use of global tests
}
```

The report goes thus:

```
──────────────────────────────
# Final report:
Failed tests:
 * TestFail
    * TestSubFail
Pass rate: 50%  1 failed  1 passed  2 total 
Average time: 0.001 ms
──────────────────────────────
```

For further customization, write thus to process the data yourself:

```C++
DATA(DataProcessor) {
  data->IterSons( [] (const lightest::Data* item) {
    // To output all the defined tests' names
    std::cout << " * " << static_cast<DataSet*>(item)->name << std::endl;
  }); // Pass a callback function to DataSet.IterSons to iterate all the test data
}
```

`data` is a pre-defined variable; its type is `const lightest::DataSet*`.

You can use `Type()` (a member function of any class extended from `Data`) and `static_cast` to transfer data's type and process the data in depth yourself. Better carefully look at `lightest.h` if you want to do this. You need to call getter functions to get data inside the instances of data classes.

All the loggings and assertions will be recorded so that you can get them while processing test data.

## Future

* Redirect outputs to `ostream&`.
* Better self testing.
* Better data analyzing & reporting system in extension of `data_analysis_ext.h`.
* More assertion macros in a independent file as an extension.
* Async testing system in a independent file as an extension.
* (Maybe) Chai like assertions support as an extension.
* Benchmark testing (time & speed test) support.
* Support installation through CMake.
* Better document for data processing API, customizing, and contribution.
* Fixture support (maybe as an extension)

## Caution

* You must add a semicolon after any assertion.
* You must add a semicolon after any definition of a sub test.
* Again, C++11 required.

## Version control

* v1.3.0 and above versions follow SemVer.
* Branch `main` will only be updated (merge from `dev` or `fix`) when publishing releases.
* Branch `fix` will be updated when fixing errors or small updates. It will be merged into `main` when publishing a PATCH version.
* Branch `dev` will be updated when adding features, big refactoring, or doing big changes. It will be merged into `main` when publishing a MAJOR or MINOR version.

## Contributing guide

### Rules

Be free to put forward issues in GitHub, and pull requests are always welcomed. Following rules should be followed.

1. Use English.
2. Every commit starts with an upper letter, and the first word should be a verb, such as `Fix`, `Add`, `Update`, `Support`, `Complete`, etc.
3. File names use underline (`_`) to separate words. Extension files have suffix of `_ext`. Test files have suffix of `_test`. Use `.cpp` and `.h`. For example, `async_test_ext.h`, `core_test.h`.
4. Variable names use lower camel case, while class, structure and function names use upper camel case. Macros are all upper case, and separate words by underline (`_`).
5. Comment start with an upper letter. Use single line comment. No dots (`.`) after a sentence ends. For example, `// Here is a comment`.
6. All source files are formatted by clang-format. Run `./format.sh` to format your source files.

### Build & test this project

To build and test this project on Linux, clone this repo and use the following commands:

```bash
mkdir build
cd build
cmake .. -Dtest=on # cmake .. -Dbenchmark=on to build benchmark test (benchmark test requires GTest pre-installed)
make -s
# To run basic tests:
cd test
./LightestCoreTest -r0 && ./LightestDataAnalysisExtTest -r0 # Make test program to return zero and not pause
# To run benchmark test:
cd benchmark
./LightestBenchmarkLightest && ./LightestBenchmarkGTest
```

Attention that basic tests should be failed, beacause failing action of **Lightest** should be tested.

### 🤝😄🤩

This project will be maintained continuously. I'll try my best to solve your problem, and make **Lightest** more fantastic together with YOU!
