// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_TESTING_TESTLIST_H_
#define OPENAGE_TESTING_TESTLIST_H_

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

#endif
