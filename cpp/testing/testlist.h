#ifndef OPENAGE_TESTING_TESTLIST_H_FECF029D4E0F4C209864E909B260D46C
#define OPENAGE_TESTING_TESTLIST_H_FECF029D4E0F4C209864E909B260D46C

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
