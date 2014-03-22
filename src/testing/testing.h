#ifndef _TESTING_TESTING_H_
#define _TESTING_TESTING_H_

//testing is the future.

/**
 * testing stuff is stored in this namespace.
 */
namespace testing {

using test_function_t = bool (*)(int, char **);

/**
 * registers a given test with a name.
 */
void register_test(const char *name, const char *description, bool interactive, test_function_t fp);

/**
 * list all registered tests.
 */
void list_tests();

/**
 * run the tests matching the given name expression.
 *
 * throws an exception if no tests are completed.
 *
 * @param expr: test name in shell globbing syntax
 * @param no_interactive: don't run tests that are marked as interactive
 * @param argc: passed to all test functions
 * @param argv: passed to all test functions
 */
void run_tests(const char *expr, bool no_interactive, int argc, char **argv);

} //namespace testing

#endif //_TESTING_TESTING_H_
