#ifndef _TESTING_TESTING_H_
#define _TESTING_TESTING_H_

//testing is the future.

#include <string>

/**
 * Openage C++ test system
 */
namespace testing {

using test_function_t = void (*)();

/**
 * print a list of all tests, including their descriptions,
 * which are loaded from the tests_cpp asset (TODO).
 */
void list_tests();

/**
 * runs the given test.
 *
 * @param name: test name
 * @returns true if the test has not failed
 */
bool run_test(std::string name);

} //namespace testing

#endif //_TESTING_TESTING_H_
