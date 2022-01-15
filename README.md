# Lightest 
Lightest, the name of the project, means the project provides a lightest C++ unit test framework. It's header only.
## Usage 
You only need to add lightest.h to your project, and include it in test files. 
 
* Use macro DEFCASE(name) to dedine a test case named 'name', and use macro RUNCASE(name) to start the test case name. In the outputs, ten '>'s and ten '<'s wrap the test case.
* Use macro DEFTEST(name) to define a test named 'name', and use macro RUNTEST(name) to start the test named 'name'. Use macro REPORTTEST() to output a test list. A test can be wrapped in a test case or run inpendently. In the outputs, ten '='s and ten '='s wrap the loggings from the test.
* Use macro MSG(str), WARN(str), ERR(str) and FAIL(str) to output test information. ERR(str) and FAIL(str) will also set the test failed. 
* Use macro LOG(varname) to output a variable. The message will include both the variable's name and its value. 
* Use macro REQUIRE(condition) to check a condition. If the condition is failed, the macro will send a Fail, or it will do nothing. The macro will return the boolean value of the condition. 
* Use macro CHECK(condition) to check a condition. If the condition is true, the macro will send a Msg, or if will send a Fail. The macro will return the boolean value of the condition. 
* The assertion macros will not output the expected value and the actual value. If you want to learn what exact value the tested expression is,
write like: 

    int a = 2;
    if(REQUIRE(1>a) LOG(a); // LOG(a) will give you the atual value of a
* You must add a semicolon after a lightest macro. 
* You should define a test before start the test. 