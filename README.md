# ![Lightest](lightest.png)

![build_pass](https://img.shields.io/badge/build-passing-green.svg)
![MIT_licensed](https://img.shields.io/badge/license-MIT-blue.svg)

**Lightest**, the name of the project, means it provides a lightest C++ unit test framework.

It's header only. If you suddenly want to do some small experiments, or decide to write a very light project ( probably like this one ), **Lightest** will be a excellent choice.

( *Your compiler has to support C++11 if you want to use this library because the library is partly based on lambda expression.*
*Also, the project has only been tested on clang++ & Ubuntu, and g++(MinGW) & Windows 10* )

## Example

Have a look at `test.cpp`. It simply shows how to use tests, assertion macros, timer macros and logging macros.

If you use MinGW (GCC & Windows) please type in Windows BAT:

```bat
g++ -std=c++11 test.cpp -o test.exe
test.exe
```

to build & run this example.

If you use GCC (Unix), please type:

```bash
g++ -std=c++11 test.cpp -o test
./test
```

If you use clang, please type:

```bash
clang++ test.cpp -o test
./test
```

Again, I want to remind you that `gccbuild.sh` & `clangbuild.sh` are merely for building the example. **No binary library files** should be built or required actually.

The output of the example should be like this below:

```
[Begin ] =====> AvgRight ----
 | [Msg  ] test.cpp:21: Pass (avg(dataSet) == expected)
[End   ] =====> AvgRight PASS
  >> TIME: 0ms
[Begin ] =====> AvgWrong ----
 | [Fail ] test.cpp:26: Didn't pass (avg_wrong(dataSet) == expected)
 |   -> EXPECTED: expected = 8.5
 |   -> ACTUAL: avg_wrong(dataSet) = 15
[End   ] ====> AvgWrong FAIL
  >> FAILURE: 1
  >> TIME: 0ms
[Begin ] =====> AvgSpeed ----
 | [Log  ] test.cpp:31: AVG_TIMER(avg(li), 10000) = 0.2062
[End   ] =====> AvgSpeed PASS
  >> TIME: 2062ms
[Report  ] --------------------
 * AvgRight: 0 failure, 0ms  ( test.cpp )
 * AvgWrong: 1 failure, 0ms  ( test.cpp )
 * AvgSpeed: 0 failure, 2062ms  ( test.cpp )
 # 1 failed tests.
[Report  ] --------------------
Done. 2077ms used.
```
(*Your time might be different*)


## Usage

You only need to add `lightest.h` to your project, and include it in test files. No static library files or other complex things needed at all!

### To add tests

* Use macro `DEFTEST(name)` to define a test named 'name'. In the outputs, ten `-` wrap the loggings from the test. `DEFTEST(name)` must be written outside functions.
* Here is an example showing how to add tests:

```C++
DEFTEST(testname) {
    int var = 123456;
    MSG("Hello from testname"); // Output a message.
    LOG(var); // Output the shared variable of var.
}
```

* All the defined tests will be automatically run if you add a macro `MAIN`. It provides a default main function which calls `lightest::Signer::TestAll()` and `lightest::Testing::ReportTotal()` to run the tests & report. If you think the outputs are too many, use the macro of `LESS_MAIN` instead of `MAIN`.

(*The test case feature is removed*)

### Ouputing macros

* Use macro `MSG(str)`, `WARN(str)`, `ERR(str)` and `FAIL(str)` to output test information. `ERR(str)` and `FAIL(str)` will also set the test failed.
* Use macro `LOG(varname)` to output a variable. The message will include both the variable's name and its value.

### Assertion macros

* Use macro `REQUIRE(condition)` to check a condition. If the condition is failed, the macro will send a Fail, or it will do nothing. The macro will return the boolean value of the condition.
* Use macro `CHECK(condition)` to check a condition. If the condition is true, the macro will send a Msg, or if will send a Fail. The macro will return the boolean value of the condition.
* If you want to learn what exact value the tested expression is, use macro `REQ_LOG(expected, actual, condition)`and `CHK_LOG(expected, actual, condition)`.
`actual` is the actual value you want to log, `expected` is the expected value you want to log, and `condition` is the condition you want to test.
* Other two assertion macros are `REQ_OP(expected, actual, operation)` and `CHK_OP(expected, actual, operation)`. They offer a easier way to compare the actual value and the expected value. You only need to give a comparing operator like `==`.
`REQ_LOG(a, b, a==b)` equals to `REQ_OP(a, b, ==)`.

Example:

```C++
REQUIRE(1<2); // Output nothing
REQUIRE(1>2); // Fail!
CHECK(1<2); // Output passed
CHECK(1>2); // Fail!
CHK_LOG(1, 2, 1==2) // Of course fail, and will also give EXPECTED:1, ACTUAL:2
CHK_OP(1, 2, ==); // Same as the one above
```

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
* Lightest will automatically catch the uncaught errors which are thrown out as `exception` or simply strings. If you want to solve the uncaught errors by your self, use the macro of `ALL_THROW()` to let Lightest throw the errors again, but that will stop the entire testing process.

Here is a simple example of a user defined main function:

```C++
int main() {
    EXCEPT(AvgSpeed); // Disable the test of AvgSpeed
    SIMPLER();
    // If you want to ignore MSGs: FILTER(lightest::MSG_LOWER);
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
 | [Msg  ] test.cpp:29: Pass (avg(dataSet) == expected)
[End   ] =====> AvgRight PASS
[Begin ] =====> AvgWrong ----
 | [Fail ] test.cpp:34: Didn't pass (avg_wrong(dataSet) == expected)
 |   -> EXPECTED: expected = 8.5
 |   -> ACTUAL: avg_wrong(dataSet) = 15
[End   ] =====> AvgWrong FAIL
[Report  ] --------------------
 * AvgRight: 0 failure, 0ms  ( test.cpp )
 * AvgWrong: 1 failure, 4ms  ( test.cpp )
 # 1 failed tests.
[Report  ] --------------------
Done. 27ms used.
```

## Caution

* You must add a semicolon after a assertion or outputing macro.
* Outputing macros and assertion macros must be used inside tests, but you can use timer macros any where.

## Future

* `MSG(...)`, `WARN(...)`, and `LOG(...)` can be used everywhere. 
* Write loggings to files
* Special output formats when writing to files

If you meet any issues, be free to put forward issues in GitHub!
(*Please use English*)
