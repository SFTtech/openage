#ifndef _TESTING_TESTLIST_H_
#define _TESTING_TESTLIST_H_

#include <map>

#include "testing.h"

namespace testing {

/**
 * list of all available tests
 */
extern std::map<std::string, test_function_t> tests;

} //namespace testing

#endif //_TESTING_REGISTER_H_
