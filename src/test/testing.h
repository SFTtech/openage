#ifndef _TEST_TESTING_H_
#define _TEST_TESTING_H_

#include <map>
#include <vector>

/**
 * testing stuff is stored in this namespace.
 */
namespace test {

/**
 * named storage for the test functions.
 */
extern std::map<const char *, bool (*)()> tests;


/**
 * registers a given test function to a test name.
 *
 * the test can later be called by that name.
 */
void register_test(const char *name, bool (*test_function)());


/**
 * list all registered tests.
 */
void list_tests();


/**
 * run the test registered to the given name.
 */
bool run_test(const char *name);


/**
 * run all tests with the given names.
 */
void run_tests(std::vector<const char *> names);


/**
 * register each existing test in this function.
 *
 * the register_test calls happen in here.
 */
void test_activation();


//list of all available testing functions:
bool test_metatest();


} //namespace test

#endif //_TEST_TESTING_H_
