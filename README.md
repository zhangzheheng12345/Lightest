# ![Lightest](https://s1.ax1x.com/2022/09/11/vO6YAs.png)

![build_pass](https://img.shields.io/badge/build-passing-green.svg)
![MIT_licensed](https://img.shields.io/badge/license-MIT-blue.svg)

**Lightest**, the name of the project, means it provides a lightest C++ unit test framework. It's also quite flexible and customizable.

The core library file is `/src/lightest.h`. Extensions will be provided in the future.

It's header only. If you suddenly want to do some small experiments, or decide to write a very light project ( probably like this one ), **Lightest** will be a excellent choice.

( *Your compiler has to support C++11 if you want to use this library because the library is partly based on lambda expression.*
*Also, the project has only been tested on clang++ in Ubuntu, and MSVC (Visual Studio) in Windows 10* )

## Example

Have a look at `test.cpp`. It simply shows how to use tests, assertion macros, timer macros, configuring macros and logging macros.

Just use CMake to build the project. Then run it. The output of the example should be like this below:

```
Failed tests:
 * TestOutputMacros
 * TestAssertionMacors
-----------------------------
[Begin ] TestOutputMacros
    [Msg  ] test.cpp:21: msg
    [Warn ] test.cpp:22: warn
    [Error] test.cpp:23: error
    [Fail ] test.cpp:24: fail
    [Log  ] test.cpp:26: a = 100
[End   ] TestOutputMacros FAIL
  >> FAILURE: 2
  >> TIME: 0ms
[Begin ] TestTimerMacoros
    [Log  ] test.cpp:31: TIMER(i++) = 0
    [Log  ] test.cpp:32: AVG_TIMER(i++, 100) = 0.00041
[End   ] TestTimerMacoros PASS
  >> TIME: 0ms
[Begin ] TestAssertionMacors
    [Fail ] test.cpp:37: Req failed
        + ACTUAL: 0
        + EXPECTED: > 0
    [Fail ] test.cpp:40: Req failed
        + ACTUAL: 0
        + EXPECTED: == 1
    [Fail ] test.cpp:40: A must didn't pass
[End   ] TestAssertionMacors FAIL
  >> FAILURE: 1
  >> TIME: 0ms
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
    MSG("Hello from Test1"); // Output a message.
    LOG(var); // Output the variable of var.
}
```

All the defined tests will be automatically run because auto registration is supported.

### Logging macros

* Use macro `MSG(str)`, `WARN(str)`, `ERR(str)` and `FAIL(str)` to output test information. `ERR(str)` and `FAIL(str)` will also set the test failed.
* Use macro `LOG(varname)` to output a variable. The message will include both the variable's name and its value.
* These macros can only be used insides tests.

### Assertion macros

Use `REQ(actual, operator, expected)`. It offers a way to compare the actual value and the expected value. If the assertion is failed, it'll output the actual value and the expected value. 
As for some situations like that you just want the returning value of a function to be true, pray write `REQ(func(), ==, true)` which is more readable and also fit Lightest.

Example:

```C++
REQ(1, ==, 2); // Fail and output actual: 1, expected: ==2
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

### Configuring macros

* You can write configurations like thus:

```C++
// in global scope
CONFIG(Config1) {
	SIMPLER();
	NO_COLOR();
	FILTER(WARN_LOWER);
	// To get command line arguments
	// argn & argc pre-defined
	for(; argn > 0; argn--, argc++) std::cout << *argc << std::endl; // Output all args
}
```

* `SIMPLER()` makes outputs' format a bit simpler.
* `NO_COLOR()` makes outputs get no coloring. This is useful when you want to write outputs to a file.
* `FILTER(level)` will ignore some outputs. For example, `FILTER(MSG_LOWER)` will ignore msg outputs (including `MSG(str)` and `LOG(var)`); `FILTER(WARN_LOWER)` will ignore msg & warn outputs. `ERR_LOWER` is also supported. Default level is `ALL`.
* You sometimes need to deal with `argn` and `argc` in configuring functions. Just use them as that you use them in main. We pre-define `argn` and `argc` in configuring functions.

## Future

* Better data processing & reporting system.
* `SUB(name)` to define sub tests.
* More assertion macros in a independent file as an extension.
* Async testing system in a independent file as an extension.
* Catch uncaught error thrown by tested programs.

## Caution

* You must add a semicolon after a assertion or outputing macro.
* Don't forget `MAIN` or `LESS_MAIN`!

If you meet any issues, be free to put forward issues in GitHub!
(*Please use English*)
