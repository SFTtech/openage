// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#pragma once


// pxd: from libcpp.string cimport string
#include <string>

#include "testing.h"


namespace openage {
namespace testing {


/**
 * Invokes the test or demo with the given name.
 *
 * pxd: void run_method(string name) except +
 */
void run_method(const std::string &name);


}} // openage::testing
