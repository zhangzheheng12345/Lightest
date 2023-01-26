![Lightest](./imgs/logo.png)

<h1 align="center">Lightest</h1>

![build_pass](https://img.shields.io/badge/build-passing-green.svg)
![MIT_licensed](https://img.shields.io/badge/license-MIT-blue.svg)

An extremely light but meanwhile strong C++ unit test framework. You should read its name as Ligh-test, with test stressed. It's headed-only, and also quite flexible and customizable.

The core library file is `/include/lightest/lightest.h`. Extensions will be provided in the future.
I promise that I'll keep the core extremely light.

Following features are supported:

* Automatic registration for testing, configuring and data processing functions.
* Assertions.
* Pretty output, include coloring (cross platform supported).
* Test data collecting.
* Timers.
* User configurations are supported if you want.

Your compiler has to support C++11. Also, the project has only been tested on clang++ on Ubuntu, and MSVC (Visual Studio) on Windows 10.

## A quick look

A short piece of test using Lightest:

```C++
#include "lightest/lightest.h"

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
      +   ACTUAL: 1
      + EXPECTED: == 2
   FAIL  test.cpp:8: REQ [a == c] failed
      +   ACTUAL: 1
      + EXPECTED: == 2
   BEGIN  SubTest
     BEGIN  SubSubTest
     PASS   SubSubTest 0.001 ms
   PASS   SubTest 0.003
 FAIL   Test 0.109 ms
Done. 1.518 ms used.
```

Outputs should be colorful if your platform is Windows, Linux or Mac.

## Usage

You only need to add `include/lightest/lightest.h` to your project in any form you like.
**Lightest** just need this to work. You can use any build system, and here we provide a suggested way to integrate **Lightest** with CMake.

### Work with CMake

A suggested way:

1. Put **Lightest** (the whole repository) into `test/` of your project.
2. Create `test/CMakeLists.txt` and add:

```CMake
# In test/CMakeLists.txt
cmake_minimum_required(VERSION 3.10) # Change the version if you need to

project(ProjectTest) # Change the name as you like

file(GLOB TEST_FILES
    "${PROJECT_SOURCE_DIR}/*.cpp") # Get all .cpp files under test/

add_executable(${PROJECT_NAME} ${TEST_FILES})
target_link_libraries(${PROJECT_NAME} lightest::lightest)

# Do not use CTest, for testing outputs should be given out by Lightest
```

3. Add `add_subdirectory(${PROJECT_SOURCE_DIR}/test)` to the `CMakeLists.txt` file in the project's root directory.
4. Use `#include "lightest/lightest.h"` in your test files created under `test/`.

But do not use CTest, for outputs should be given out by **Lightest**, while CTest will 'eat' others' outputs.

### To add tests

Use macro `TEST(name)` to define a test named 'name'.

Here is an example showing how to add tests:

```C++
// in global scope
TEST(Test1) {
  REQ(1, ==, 1); // Pass, of course
}
```

Use macro `SUB(name)` to add sub tests. Sub test in sub test is allowed.

Example:

```C++
// inside global tests or sub tests
SUB(SubTest1) {
  REQ(1, ==, 1); // Pass
  SUB(SubInSub1) {
    REQ(1, ==, 1); // Pass
  }; // semicolon required
}; // semicolon required
```

All the defined tests will be automatically run because auto registration is supported.

### Assertion macros

Use `REQ(actual, operator, expected)` to compare the actual value and the expected value. If the assertion fails, it'll output the actual value and the expected value. 

As for some situations like that you just want the returning value of a function to be true, pray write `REQ(func(), ==, true)` which is more readable and also fit **Lightest**.

We also provide `MUST(condition)`. The current test will be stopped if `condition` is false.

Example:

```C++
// in a test
int a = 1, b = 1, c = 2;
REQ(a, ==, b); // Pass with no output -- The cleaner, the better.
REQ(a, ==, c); // Fail and output:
//  REQ [a == c] failed
//      + ACTUAL: 1
//      + EXPECTED: == 2
MUST(REQ(1, ==, 2)); // Fail and current test will be stopped
```

### Timer macros

* `TIMER(sentence)` runs the sentence provided, and returns how long did the sentence spend running. It returns type `clock_t`, ms.
* `AVG_TIMER(sentence, times)` runs the sentence `times` times, and then returns the average time.
* The time unit is always minisecond(ms).

Example:

```C++
TIMER(std::cout << "One Hello" << std::endl); // Return how long the sentence spends running
TIMER(std::cout << "Avg Hello" << std::endl, 1000); // Run it 1000 times and return the average time
```

### Configuring macros

You can write configurations thus (`CONFIG` functions will be run before tests):

```C++
// in global scope
CONFIG(Config1) {
  NO_COLOR();
  // To get command line arguments
  // argn & argc pre-defined
  for(; argn > 0; argn--, argc++)
    std::cout << *argc << std::endl; // Output all args
}
```

* `NO_COLOR()` makes outputs get no coloring. Useful when you want to write outputs to a file.
* `NO_OUTPUT()` forbids the default outputting system to give out the loggings. Useful when you only want to deal the test data yourself and don't want any default output.
* `argn` and `argc` are pre-defined in configuring functions.

### To process data yourself

Write thus to process the data yourself:

```C++
DATA(DataProcessor1) {
  data->IterSons( [] (const lightest::Data* item) {
    // To output all the defined tests' names
    std::cout << " * " << static_cast<DataSet*>(item)->name << std::endl;
  }); // Pass a callback function to DataSet.IterSons to iterate all the test data
}
```

`data` is a pre-defined variable; its type is `const lightest::DataSet*`.

You can use method of `Type()` and `static_cast` to transfer data's type and process the data deeply yourself. 
Better carefully look at `lightest.h` if you want to do this.
You need to call getter functions to get data inside the instances of data classes.

All the loggings and assertions will be recorded so that you can get them while processing test data.

## Future

* Redirect outputs to `ostream&`.
* Better data processing & reporting system.
* More assertion macros in a independent file as an extension.
* Async testing system in a independent file as an extension.
* Catch uncaught error thrown out by tests.
* (Maybe) Chai like assertions support as an extension.
* A stronger data processing tool set as an extension.
* Benchmark testing (time & speed test) support.
* Support installation through CMake.
* Better document for data processing API, customizing, and contribution.
* Fixture support (maybe as an extension)
* Tests in different files will be devided into different block while outputting.

## Caution

* You must add a semicolon after a assertion.
* You must add a semicolon after a definition of a sub test.
* Again, C++11 required.

## Version control

* v1.3.0 and above versions follow SemVer.
* Branch `main` will only be updated (merge from `dev` or `fix`) when publishing releases.
* Branch `fix` will be updated when fixing errors or small updates. It will be merged into `main` when publishing a PATCH version.
* Branch `dev` will be updated when adding features, big refactoring, or doing big changes. It will be merged into `main` when publishing a MAJOR or MINOR version.

## Contributing

Be free to put forward issues in GitHub, and pull requests are always welcomed. Following rules should be followed.

1. Use English.
2. Every commit starts with an upper letter, and the first word should be a verb, such as `Fix`, `Add`, `Update`, `Support`, `Complete`, etc.
3. File names use underline (`_`) to separate words. Extension files have suffix of `_ext`. Use `.cpp` and `.h`. For example, `async_test_ext.h`.
4. Variable names use lower camel case, while class, structure and function names use upper camel case. Macros are all upper case, and separate words by underline (`_`).
5. Comment start with an upper letter. Use single line comment. No dots (`.`) after a sentence ends. For example, `// Here is a comment`.
6. All source files are formatted by clang-format.

This project will be maintained continuously. I'll try my best to solve your problem, and make **Lightest** more fantastic together with YOU!
