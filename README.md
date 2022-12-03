# ![Lightest](https://s1.ax1x.com/2022/09/11/vO6YAs.png)

![build_pass](https://img.shields.io/badge/build-passing-green.svg)
![MIT_licensed](https://img.shields.io/badge/license-MIT-blue.svg)

**Lightest**, the name of the project, means it provides a lightest C++ unit test framework.

The library file is `/src/lightest.h`.

It's header only. If you suddenly want to do some small experiments, or decide to write a very light project ( probably like this one ), **Lightest** will be a excellent choice.

( *Your compiler has to support C++11 if you want to use this library because the library is partly based on lambda expression.*
*Also, the project has only been tested on clang++ in Ubuntu, and g++(MinGW) in Windows 10* )

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

* All the defined tests will be automatically run if you add a macro `MAIN`. It provides a default main function which calls `lightest::Register::TestAll()` and `lightest::Testing::ReportTotal()` to run the tests & report. If you think the outputs are too many, use the macro of `LESS_MAIN` instead of `MAIN`.

(*The test case feature is removed*)

### Ouputing macros

* Use macro `MSG(str)`, `WARN(str)`, `ERR(str)` and `FAIL(str)` to output test information. `ERR(str)` and `FAIL(str)` will also set the test failed.
* Use macro `LOG(varname)` to output a variable. The message will include both the variable's name and its value.

### Assertion macros

* Use macro `REQUIRE(condition)` to check a condition. If the condition is failed, the macro will send a Fail, or it will do nothing.
* If you want to learn what exact value the tested expression is, use macro `REQ_LOG(expected, actual, condition)`and `CHK_LOG(expected, actual, condition)`.
* Another assertion macro is `REQ_OP(expected, actual, operation)`. It offers a easier way to compare the actual value and the expected value. You only need to give a comparing operator like `==`.
`REQ_LOG(a, b, a==b)` equals to `REQ_OP(a, b, ==)`.
* `REQ_ARR(expected, actual, expLen, actualLen, operator)` can compare two arrays. Besides the basic information, it will also put the index of the unexpected value.

Example:

```C++
REQUIRE(1<2); // Output nothing
REQUIRE(1>2); // Fail!
REQ_LOG(1, 2, 1==2); // Of course fail, and will also give EXPECTED:1, ACTUAL:2
REQ_OP(1, 2, ==); // Same as the one above
int arr1[] = {1, 2, 3}, arr2[] = {1, 2, 3};
REQ_ARR(arr1, arr2, 3, 3, ==); // Obviously pass
```

* There used to be a macro set called `CHECK`. It is removed because I think it's actually useless, making this library weigh too much.

### Timer macros

* `TIMER(sentence)` runs the sentence provided, and returns how long did the sentence spend running. (*It returns type `clock_t`, ms*)
* `AVG_TIMER(sentence, times)` runs the sentence provided `times` times, and then returns the average time.

Example:

```C++
TIMER(std::cout << "One Hello" << std::endl); // Return how long the sentence spends running
TIMER(std::cout << "Avg Hello" << std::endl, 1000); // Run it 1000 times and return the average time
```

### Your main function

Besides `MAIN` and `LESS_MAIN`, you can write your own main function.

* The macro of `TESTALL()` runs all the signed tests and clear the signed test list.
* The macro of `REPORT()` gives a total report.
* The macro of `SIMPLER()` hide the `>> TIME: ...ms` and `>> FAILURE: ...` (if there is) at the end of each test.
* If you don't want to run some of the tests, use the macro of `EXCEPT(name)` to disable them.
* If you don't want to see some of the outputs, use the macro of `FILTER(level)`. The `level` should be `lightest::MSG_LOWER`, `lightest::WARN_LOWER` or `lightest::ERR_LOWER`. `lightest::MSG_LOWER` ignores MSGs, `lightest::WARN_LOWER` ignores MSGs & WARNs, and `lightest::ERR_LOWER` ignores MSGs, WARNs & ERRORs. Outputs of `LOG(varname)` is at the same level of MSGs.
* Lightest will automatically catch the uncaught errors which are thrown out as `exception`, simple strings as well as other user-defined error types. If you want to solve the uncaught errors by your self, use the macro of `ALL_THROW()` to let Lightest throw the errors again, but that will stop the entire testing process.
* Lightest has a simple coloring system which supports Windows, Linux, and MacOS. If you want to write outputs to files, please add the macro of `NO_COLOR()` in main function or the first test you defined. It will close the coloring system so that messy code will not appear in your files. (This problem will probably happen in Unix platforms.)

Here is a simple example of a user-defined main function:

```C++
int main() {
    EXCEPT(AvgSpeed); // Disable the test of AvgSpeed
    SIMPLER();
    // If you want to ignore MSGs: FILTER(lightest::MSG_LOWER);
    // If you want to write to files: NO_COLOR();
    TESTALL();
    /*
    If you want to deal with the errors by your self:
    ALL_THROW();
    try { TESTALL(); } catch(...) {...}
    */
    REPORT();
    return 0; // Don't forget `return 0`!
}
```

Replace the `MAIN` macro in `test.cpp` and then run it. The output will be like this below now:

```
[Begin ] =====> AvgRight ----
[End   ] =====> AvgRight PASS
[Begin ] =====> AvgWrong ----
 | [Fail ] example.cpp:32: Didn't pass ((TestData::expected) == (avg_wrong(TestData::dataSet)))
 |   |-> EXPECTED: (TestData::expected) = 8.5
 |   |-> ACTUAL: (avg_wrong(TestData::dataSet)) = 15
[End   ] =====> AvgWrong FAIL
Done. 643276ms used.
```

## Caution

* You must add a semicolon after a assertion or outputing macro.
* `ERR(str)`, `FAIL(str)`, and assertion macros must be used inside tests, but you can use timer macros and `MSG(str)`, `WARN(str)`, and `LOG(varname)` any where.

If you meet any issues, be free to put forward issues in GitHub!
(*Please use English*)
