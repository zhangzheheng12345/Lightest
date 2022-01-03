# Lightest 
Lightest, the name of the project, means the project provides a lightest C++ unit test framework. It's header only.
## Usage 
You only need to add lightest.h to your project, and include it in test files. 
 
* Use macro DEFTEST(name) to define a test named 'name', and use macro RUNTEST(name) to start the test named 'name'. Use macro REPORTTEST() to output a test list. 
* Use macro MSG(str), WARN(str), ERR(str) and FAIL(str) to output test information. ERR(str) and FAIL(str) will set the test failed. 
* Use macro REQUIRE(condition) to check a condition. If the condition is failed, the macro will send a Fail, or it will do nothing. 
* Use macro CHECK(condition) to check a condition. If the condition is true, the macro will send a Msg, or if will send a Fail. 
* You must add a semicolon after a lightest macro. 
* You should define a test before start the test. 