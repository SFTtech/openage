#ifndef _TESTING_TESTLIST_H_
#define _TESTING_TESTLIST_H_

#include <map>

#include "testing.h"

namespace testing {

/**
 * list of all available tests
 */
extern std::map<std::string, test_function_t> tests;

/**
 * list of all available demos
 */
extern std::map<std::string, demo_function_t> demos;

} //namespace testing

#endif //_TESTING_REGISTER_H_
