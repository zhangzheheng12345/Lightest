# ![Lightest](https://s1.ax1x.com/2022/09/11/vO6YAs.png)

![build_pass](https://img.shields.io/badge/build-passing-green.svg)
![MIT_licensed](https://img.shields.io/badge/license-MIT-blue.svg)

**Lightest**, the name of the project, means it provides a extremely light but meanwhile string C++ unit test framework. It's also quite flexible and customizable.

The core library file is `/src/lightest.h`. Extensions will be provided in the future.

It's header only. If you suddenly want to do some small experiments, or decide to write a very light project ( probably like this one ), **Lightest** will be a excellent choice.

Following features are supported:

* Automatic registration for testing, configuring and data processing functions.
* Loggers such as MSG and FAIL.
* Assertions.
* Pretty output, include coloring (cross platform supported).
* Test data collecting.
* Timers.
* Configurations are supported if you want.

( *Your compiler has to support C++11. Also, the project has only been tested on clang++ in Ubuntu, and MSVC (Visual Studio) in Windows 10* )

## Example

Have a look at `test.cpp`. It simply shows how to use tests, assertion macros, timer macros, configuring macros and logging macros.

Just use CMake to build the project. Then run it. The output of the example should be like this below:

```
Failed tests:
 * TestOutputMacros
 * TestAssertionMacors
-----------------------------
[Begin ] TestOutputMacros
    [Msg  ] test.cpp:20: msg
    [Warn ] test.cpp:21: warn
    [Error] test.cpp:22: error
    [Fail ] test.cpp:23: fail
    [Log  ] test.cpp:25: a = 100
[End   ] TestOutputMacros FAIL 0ms
[Begin ] TestTimerMacoros
    [Log  ] test.cpp:30: TIMER(i++) = 0
    [Log  ] test.cpp:31: AVG_TIMER(i++, 100) = 0.00059
[End   ] TestTimerMacoros PASS 0ms
[Begin ] TestAssertionMacors
    [Fail ] test.cpp:36: Req failed
        + ACTUAL: 0
        + EXPECTED: > 0
    [Fail ] test.cpp:39: Req failed
        + ACTUAL: 0
        + EXPECTED: == 1
    [Fail ] test.cpp:39: A must didn't pass
[End   ] TestAssertionMacors FAIL 0ms
Done. 0ms used.

```

(*Your time & file path might be different,*
*and they should be colorful if your platform is Windows, Linux or Mac*)

## Usage

You only need to add `lightest.h` to your project, and include it in test files. No static library files or other complex things needed at all!

### To add tests

Use macro `TEST(name)` to define a test named 'name'.

Here is an example showing how to add tests:

```C++
// in global scope
TEST(Test1) {
    int var = 123456;
    MSG("Hello from Test1");
    LOG(var);
}
```

All the defined tests will be automatically run because auto registration is supported.

### Logging macros

* Use macro `MSG(str)`, `WARN(str)`, `ERR(str)` and `FAIL(str)` to output test information. `ERR(str)` and `FAIL(str)` will also set the test failed.
* Use macro `LOG(varname)` to output a variable. The message will include both the variable's name and its value.
* These macros can only be used insides tests.

Example:

```C++
// in a test
MSG("Hello? This is a message.");
WARN("Hey! This is a warning.");
ERR("Opps! An error occurred."); // Output & set the test failed
FAIL("Oh no! Test failed."); // Output & set the test failed
int a = 100;
LOG(a); // Output variable's name and its value at the same time
``` 

### Assertion macros

Use `REQ(actual, operator, expected)`. It offers a way to compare the actual value and the expected value. If the assertion is failed, it'll output the actual value and the expected value. 

As for some situations like that you just want the returning value of a function to be true, pray write `REQ(func(), ==, true)` which is more readable and also fit Lightest.

We also provide `MUST(condition)`. The current test will be stopped if `condition` is false.

Example:

```C++
// in a test
REQ(1, ==, 1); // Pass with no output -- The cleaner, the better.
REQ(1, ==, 2); // Fail and output actual: 1, expected: ==2
MUST(REQ(1, ==, 2)); // Fail and current test will be stopped
```

### Timer macros

* `TIMER(sentence)` runs the sentence provided, and returns how long did the sentence spend running. (*It returns type `clock_t`, ms*)
* `AVG_TIMER(sentence, times)` runs the sentence provided `times` times, and then returns the average time.
* The time unit is always minisecond(ms).

Example:

```C++
TIMER(std::cout << "One Hello" << std::endl); // Return how long the sentence spends running
TIMER(std::cout << "Avg Hello" << std::endl, 1000); // Run it 1000 times and return the average time
```

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
Better carefully look at `lightest.h` if you want to do this. Const values can be got directly while for some other you need to call a getter function.

All the loggings and assertions will be recorded so that you can get them while processing test data.

### Configuring macros

You can write configurations like thus (`CONFIG` functions will be run before tests):

```C++
// in global scope
CONFIG(Config1) {
    NO_COLOR();
    FILTER(WARN_LOWER);
    // To get command line arguments
    // argn & argc pre-defined
    for(; argn > 0; argn--, argc++) std::cout << *argc << std::endl; // Output all args
}
```

* `NO_COLOR()` makes outputs get no coloring. This is useful when you want to write outputs to a file.
* `FILTER(level)` will ignore some outputs. For example, `FILTER(MSG_LOWER)` will ignore msg outputs (including `MSG(str)` and `LOG(var)`); `FILTER(WARN_LOWER)` will ignore msg & warn outputs. `ERR_LOWER` is also supported. Default level is `ALL`.
* `NO_OUTPUT()` forbids the default outputting system to give out the loggings. This is useful when you only want to deal the test data yourself and don't want any default output.
* You sometimes need to deal with `argn` and `argc` in configuring functions. Just use them as that you use them in main. We pre-define `argn` and `argc` in configuring functions.

## Future

* Better data processing & reporting system.
* `SUB(name)` to define sub tests.
* More assertion macros in a independent file as an extension.
* Async testing system in a independent file as an extension.
* Catch uncaught error thrown by tested programs.
* (Maybe) Chai like assertions support as an extension.
* A stronger data processing tool set as an extension.

## Caution

* You must add a semicolon after a assertion or outputing macro.
* Logging macros and assertions macros can only be used in tests. 

If you meet any issues, be free to put forward issues in GitHub!
(*Please use English*)
