// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_UNIQUE_IDENTIFIER_H_
#define OPENAGE_UTIL_UNIQUE_IDENTIFIER_H_

namespace openage {
namespace util {

/**
 * returns a different size_t value for every set of template parameters.
 */
template<typename ...>
size_t unique_value() {
	static char mem;
	return reinterpret_cast<size_t>(&mem);
}

/**
 * returns a different storage location of type T for every set of
 * template paramters.
 */
template<typename T, typename ...>
T *unique_memorycell() {
	static T mem;
	return &mem;
}

// local linkage: for every translation unit, different unique values are
// returned.
namespace {

/**
 * returns a different size_t value for every set of template paramters
 * and every translation unit.
 */
template<typename ...>
size_t tu_unique_value() {
	static char mem;
	return reinterpret_cast<size_t>(&mem);
}

/**
 * returns a different storage location of type T for every set of
 * template paramters and every translation unit.
 */
template<typename T, typename ...>
T *unique_memorycell() {
	static T mem;
	return &mem;
}

} // anonymous namespace

namespace unique_identifier_helpers {

/**
 * The above function templates only accept typenames;
 * to pass values, you can convert them to types using this class.
 *
 * example type for int 42:
 *
 * unique_identifier_value_helper_type<int, 42>
 */
template<typename T, T val>
class value_as_type {
	// don't even think about instantiating this type.
	value_as_type() = delete;

public:
	static constexpr T value = val;
};

/**
 * This class exists only to function as an extra unique typename input
 * for the macro below. It should not be used outside the macro.
 */
class another_type {
	// don't even think about instantiating this type.
	another_type() = delete;
};

} // namespace unique_identifier_helpers

#define UNIQUE_VALUE openage::util::tu_unique_value<openage::util::unique_identifier_helpers::value_as_type<int, __COUNTER__>, openage::util::unique_identifier_helpers::another_type>()

}} // namespace openage::util

#endif
