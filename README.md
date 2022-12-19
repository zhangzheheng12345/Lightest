# ![Lightest](https://s1.ax1x.com/2022/09/11/vO6YAs.png)

![build_pass](https://img.shields.io/badge/build-passing-green.svg)
![MIT_licensed](https://img.shields.io/badge/license-MIT-blue.svg)

**Lightest**, the name of the project, means it provides a lightest C++ unit test framework.

The core library file is `/src/lightest.h`.

It's header only. If you suddenly want to do some small experiments, or decide to write a very light project ( probably like this one ), **Lightest** will be a excellent choice.

( *Your compiler has to support C++11 if you want to use this library because the library is partly based on lambda expression.*
*Also, the project has only been tested on clang++ in Ubuntu, and MSVC (Visual Studio) in Windows 10* )

## Example

Have a look at `example.cpp`. It simply shows how to use tests, assertion macros, timer macros and logging macros.

If you want to run the example, change the variable `build_example` in `CMakeLists.txt` from `false` to `true`, and then use CMake to build this project.

The output of the example should be like this below:

```
[Begin ] =====> AvgRight ----
[End   ] =====> AvgRight PASS
  >> TIME: 3ms
[Begin ] =====> AvgWrong ----
 | [Fail ] example.cpp:32: Didn't pass ((TestData::expected) == (avg_wrong(TestData::dataSet)))
 |   |-> EXPECTED: (TestData::expected) = 8.5
 |   |-> ACTUAL: (avg_wrong(TestData::dataSet)) = 15
[End   ] =====> AvgWrong FAIL
  >> FAILURE: 1
  >> TIME: 56ms
[Begin ] =====> AvgSpeed ----
 | [Log  ] example.cpp:37: AVG_TIMER(avg(li), 10000) = 63.4643
[End   ] =====> AvgSpeed PASS
  >> TIME: 641180ms
[Report  ] --------------------
 * AvgRight: 0 failure, 3ms  ( example.cpp )
 * AvgWrong: 1 failure, 56ms  ( example.cpp )
 * AvgSpeed: 0 failure, 641180ms  ( example.cpp )
 # [ 2 / 3 ] passed
 # { 1 } failures occurred
[Report  ] --------------------
Done. 643276ms used.
```
(*Your time & file path might be different,*
*and they should be colorful if your platform is Windows, Linux or Mac*)

## Usage

You only need to add `lightest.h` to your project, and include it in test files. No static library files or other complex things needed at all!

### To add tests

* Use macro `TEST(name)` to define a test named 'name'. In the outputs, ten `-` wrap the loggings from the test. `TEST(name)` must be written outside functions.
* Here is an example showing how to add tests:

```C++
TEST(testname) {
    int var = 123456;
    MSG("Hello from testname"); // Output a message.
    LOG(var); // Output the shared variable of var.
}
```

* All the defined tests will be automatically run if you add a macro `MAIN`. If you think the outputs are too many, use the macro of `LESS_MAIN` instead of `MAIN`.

### Ouputing macros

* Use macro `MSG(str)`, `WARN(str)`, `ERR(str)` and `FAIL(str)` to output test information. `ERR(str)` and `FAIL(str)` will also set the test failed.
* Use macro `LOG(varname)` to output a variable. The message will include both the variable's name and its value.

### Assertion macros

Use `REQ(actual, operator, expected)`. It offers a way to compare the actual value and the expected value. If the assertion is failed, it'll output the actual value and the expected value. As for some situations like that you just want the returning value of a function to be true, pray write `REQ(func(), ==, true)` which is more readable and also fit Lightest.

Example:

```C++
REQ(1, ==, 2); // Fail and output actual: 1, expected: ==2
```

### Timer macros

* `TIMER(sentence)` runs the sentence provided, and returns how long did the sentence spend running. (*It returns type `clock_t`, ms*)
* `AVG_TIMER(sentence, times)` runs the sentence provided `times` times, and then returns the average time.

Example:

```C++
TIMER(std::cout << "One Hello" << std::endl); // Return how long the sentence spends running
TIMER(std::cout << "Avg Hello" << std::endl, 1000); // Run it 1000 times and return the average time
```

## Caution

* You must add a semicolon after a assertion or outputing macro.
* `ERR(str)`, `FAIL(str)`, and assertion macros must be used inside tests, but you can use timer macros and `MSG(str)`, `WARN(str)`, and `LOG(varname)` any where.

If you meet any issues, be free to put forward issues in GitHub!
(*Please use English*)
