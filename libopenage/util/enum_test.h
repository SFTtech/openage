// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libopenage.util.enum cimport Enum, EnumValue
#include "enum.h"

namespace openage {
namespace util {
namespace tests {

/**
 * pxd:
 *
 * cppclass testenum_value(EnumValue[testenum_value, int]):
 *     int numeric
 *     const char *name
 *     const char *stuff
 */
struct OAAPI testenum_value : EnumValue<testenum_value> {
	const char *stuff;

	const char *get_stuff() const {
		return stuff;
	}
};

/**
 * pxd:
 *
 * cppclass testenum(Enum[testenum_value]):
 *     testenum_value *value
 *
 * testenum foo "::openage::util::tests::testenum::foo"
 * testenum bar "::openage::util::tests::testenum::bar"
 */
struct OAAPI testenum : Enum<testenum_value> {
	// this is always required for classes which inherit from Enum.
	// it allows the constructors to work as expected.
	using util::Enum<testenum_value>::Enum;

	// a default constructor for testenum.
	// this is needed if we wish to allow use from Cython.
	// initializes the testenum to an internal UNDEFINED value.
	testenum();

	static constexpr testenum_value foo{{"foo", 1}, "foooooooooooooooooo"};
	static constexpr testenum_value bar{{"bar", 2}, "barrrrrrrrrrrrrrrrr"};
};

}}} // openage::util::tests
