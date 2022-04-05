# ![Lightest!](lightest.png)

![](https://img.shields.io/badge/build-passing-green.svg)
![](https://img.shields.io/badge/license-MIT-blue.svg)

**Lightest**, the name of the project, means it provides a lightest C++ unit test framework.

It's header only ( really, only a header file, without any binary files required ). If you suddenly want to do some small experiments, or decide to write a pretty light project ( probably like this one ), **Lightest** will be a excellent choice.
The framework is based on lambda expression, so your compiler has to support C++11.

( *The project has only been tested on clang++ & Ubuntu, and g++(MinGW) & Windows 10* )

## Example

Have a look at `test.cpp`. It simply shows how to use test case, test, assertion macros, and logging macros. 

If you use GCC, please type:

```bash
$ ./gccbuild.sh
$ ./test.out
```

to build & run this example.

If you use clang, please type:

```batch
$ ./clangbuild.sh
$ ./test.out
```

Again, I want to remind you that `gccbuild.sh` & `clangbuild.sh` are **merely for building the example**. **No binary library files** should be built or required actually.

The output of the example should be like this below:

```
[Begin ] -------------------- AvgRight
 | [Msg  ] test.cpp:13: Pass (avg(dataSet) == expected)
 |                     { CHECK }
 | [Log  ] test.cpp:14: AVG_TIMER(avg(dataSet), 200) = 0.005
[End   ] -------------------- AvgRight PASS
  >> TIME: 1ms
[Begin ] -------------------- AvgWrong
 | [Fail ] test.cpp:18: Didn't pass (avg_wrong(dataSet) == expected)
 |                     { CHECK }
 |                     #ACTUAL: avg_wrong(dataSet) = 15
 | [Log  ] test.cpp:19: AVG_TIMER(avg(dataSet), 200) = 0.005
[End   ] -------------------- AvgWrong FAIL
  >> FAILURE: 1
  >> TIME: 1ms
[Report  ] -------------------- TOTAL
 * AvgCase.AvgRight: 0 failure, 1ms  ( test.cpp )
 * AvgCase.AvgWrong: 1 failure, 1ms  ( test.cpp )
 # 0 failed tests.
[Report  ] -------------------- TOTAL
Done. 2ms used.
```

## Usage

You only need to add `lightest.h` to your project, and include it in test files. No static library files or other complex things needed at all!

### To add tests

* Use macro `DEFTEST(name)` to define a test named 'name'. In the outputs, ten `-` wrap the loggings from the test. `DEFTEST(name)` must be written in functions.
* Here is an example showing how to add tests:

```C++
DEFTEST(testname) {
    int var = 123456;
    MSG("Hello from testname"); // Output a message.
    LOG(var); // Output the shared variable of var.
};
```

* All the defined tests will be automatically run.
  
  You can also call `testing.TestAll()` out of tests and test cases.`testing.TestAll()` will run the signed tests clear the signing list without giving a total report.

* A total report will be automatically provided in the end.
(*The test case feature is removed*)

### Ouputing macros

* Use macro `MSG(str)`, `WARN(str)`, `ERR(str)` and `FAIL(str)` to output test information. `ERR(str)` and `FAIL(str)` will also set the test failed.
* Use macro `LOG(varname)` to output a variable. The message will include both the variable's name and its value.

### Assertion macros

* Use macro `REQUIRE(condition)` to check a condition. If the condition is failed, the macro will send a Fail, or it will do nothing. The macro will return the boolean value of the condition.
* Use macro `CHECK(condition)` to check a condition. If the condition is true, the macro will send a Msg, or if will send a Fail. The macro will return the boolean value of the condition.
* The assertion macros will not output the expected value and the actual value. If you want to learn what exact value the tested expression is, use macro `REQ_LOG(varname, condition)`and `CHK_LOG(varname, condition)`. `varname` is the variable you want to log and `condition` is the condition you want to test.
  In old versions, write like below to get the same result:

```C++
int a = 2;
REQ_LOG(1>a) // Newer version: REQ_LOG(varname, condition) and CHK_LOG(varname, condition) supported
```

### Timer macros

* `TIMER(sentence)` runs the sentence provided, and returns how long did the sentence spend running. (*It returns type `clock_t`, ms*)
* `AVG_TIMER(sentence, times)` runs the sentence provided `times` times, and then returns the average time.

## Caution

* You must add a semicolon after a **Lightest** macro.
* Outputing macros and assertion macros must be used inside tests, but you can use timer macros any where.
* If you meet any issue, please have a look at the source code or put forward an issue.

## Future

* Lighter, lighter, and lighter. I found this framework is becoming heavier, gradually running off course.
  Writing one single test needs a lot of codes, which make me decide to make it lighter & lighter. 
* Write loggings to files
* Special output formats when writing to files
