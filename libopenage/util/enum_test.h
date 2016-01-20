// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp.string cimport string
#include <string>

// pxd: from libopenage.util.enum cimport Enum
#include "enum.h"

namespace openage {
namespace util {
namespace tests {


/**
 * pxd:
 *
 * cppclass testtype_data:
 *     string stuff
 */
struct testtype_data {
	std::string stuff;
};


// I heavily suggest using a typedef like this.
// Nobody wants to type "Enum<testtype_data>" whenever using this.

// pxd: ctypedef Enum[testtype_data] testtype
using testtype = Enum<testtype_data>;


namespace testtypes {

// Those need to be defined in a cpp file, unfortunately.

// pxd: testtype foo
// pxd: testtype bar
extern testtype foo;
extern testtype bar;

} // testtypes


}}} // openage::util::tests
