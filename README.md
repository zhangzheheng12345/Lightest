# ![Lightest](https://s1.ax1x.com/2022/09/11/vO6YAs.png)

![build_pass](https://img.shields.io/badge/build-passing-green.svg)
![MIT_licensed](https://img.shields.io/badge/license-MIT-blue.svg)

**Lightest**, the name of the project, means it provides a extremely light but meanwhile string C++ unit test framework. It's also quite flexible and customizable.

The core library file is `/src/lightest.h`. Extensions will be provided in the future. I promise that I'll keep the core extremely light.

It's header only. If you suddenly want to do some small experiments, or decide to write a very light project ( probably like this one ), **Lightest** will be a excellent choice.

Following features are supported:

* Automatic registration for testing, configuring and data processing functions.
* Assertions.
* Pretty output, include coloring (cross platform supported).
* Test data collecting.
* Timers.
* Configurations are supported if you want.

( *Your compiler has to support C++11. Also, the project has only been tested on clang++ in Ubuntu, and MSVC (Visual Studio) in Windows 10* )

Take a quick look at how will Lightest output, similar to below, simple and tidy:

```
[Begin ] TestTimerMacoros
[End   ] TestTimerMacoros PASS 0ms
[Begin ] TestAssertionMacors
    [Fail ] test.cpp:23: REQ [a > b] failed
        + ACTUAL: 0
        + EXPECTED: > 0
    [Fail ] test.cpp:26: REQ [a == c] failed
        + ACTUAL: 0
        + EXPECTED: == 1
[End   ] TestAssertionMacors FAIL 0ms
Done. 0ms used.
```

## Example

Have a look at `test.cpp`. It simply shows how to use tests, assertion macros, timer macros, and configuring macros.

Just use CMake to build the project. Then run it. The outputs should be like those given out above, but maybe a bit more, since `test.cpp` adds a user defined data processor to list the failed tests.

(*they should be colorful if your platform is Windows, Linux or Mac*)

## Usage

You only need to add `lightest.h` to your project, and include it in test files. No static library files or other complex things needed at all!

### To add tests

Use macro `TEST(name)` to define a test named 'name'.

Here is an example showing how to add tests:

```C++
// in global scope
TEST(Test1) {
    REQ(1, ==, 1); // Pass, of course
}
```

All the defined tests will be automatically run because auto registration is supported.

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
    // To get command line arguments
    // argn & argc pre-defined
    for(; argn > 0; argn--, argc++) std::cout << *argc << std::endl; // Output all args
}
```

* `NO_COLOR()` makes outputs get no coloring. This is useful when you want to write outputs to a file.
* `NO_OUTPUT()` forbids the default outputting system to give out the loggings. This is useful when you only want to deal the test data yourself and don't want any default output.
* You sometimes need to use `argn` and `argc` in configuring functions. Just use them as that you use them in main. We pre-define `argn` and `argc` in configuring functions.

## Future

* Better data processing & reporting system.
* `SUB(name)` to define sub tests.
* More assertion macros in a independent file as an extension.
* Async testing system in a independent file as an extension.
* Catch uncaught error thrown by tested programs.
* (Maybe) Chai like assertions support as an extension.
* A stronger data processing tool set as an extension.

## More

* You must add a semicolon after a assertion or outputing macro.

If you meet any issues, be free to put forward issues in GitHub!
(*Please use English*)
