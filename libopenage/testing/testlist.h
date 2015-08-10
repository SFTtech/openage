// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_TESTING_TESTLIST_H_
#define OPENAGE_TESTING_TESTLIST_H_

#include "testing.h"

// pxd: from libcpp.string cimport string
// pxd: from libcpp.vector cimport vector


namespace openage {
namespace testing {


/**
 * Invokes the test or demo with the given name.
 *
 * pxd: void run_method(string name) except +
 */
void run_method(const std::string &name);


}} // openage::testing

#endif
