// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_TESTING_TESTING_H_
#define OPENAGE_TESTING_TESTING_H_

//testing is the future.

#include <string>

/**
 * Openage C++ test system
 */
namespace testing {

using test_function_t = void (*)();
using demo_function_t = void (*)(int, char **);

/**
 * lists all tests and demos, including their descriptions,
 * which are loaded from the tests_cpp asset (TODO).
 */
void list_tests();

/**
 * runs the given test.
 *
 * @param name: test name
 * @returns false if the test didn't exist or has failed
 */
bool run_test(const std::string &name);

/**
 * runs the given demo.
 *
 * @param name: demo name
 * @returns false if the demo didn't exist or has failed
 */
bool run_demo(const std::string &name, int argc, char **argv);

} //namespace testing

#endif
