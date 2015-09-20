// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdlib>


namespace openage {
namespace util {


/**
 * Evaluate `value` at compiletime and return it.
 * This can force constexpr evaluation.
 */
template<typename T, T value>
constexpr inline T compiletime() {
	return value;
}


/**
 * this namespace contains constexpr functions, i.e. C++11 functions that are designed
 * to run at compile-time.
 *
 * they're mostly ugly and use recursion because that's how C++11 works.
 * once C++14 support is widespread on the various target platforms, they can be made
 * much more readable.
 */
namespace constexpr_ {


/**
 * returns true IFF the string literals have equal content.
 */
constexpr bool streq(const char *a, const char *b) {
	// if only c++14 had arrived a little bit earlier...

	return

	*a != *b ?
		// if chars differ:
		false
	: *a == '\0' ?
		// elif strs are over:
		true
	:
		// else:
		streq(a + 1, b + 1);
}


/**
 * returns the length of the string literal, excluding the terminating NULL byte.
 *
 * @param len   add this value to the result (used internally for recursing)
 */
constexpr size_t strlen(const char *str, size_t len = 0) {
	// if only c++14 had arrived a little bit earlier...

	return

	*str == '\0' ?
		// if str is over:
		len
	:
		// else:
		strlen(str + 1, len + 1);
}

/**
 * stores a string literal plus a "length specifier".
 *
 * due to the nature of C strings, parts can only be cut off at the start of the string;
 * this struct represents a string literal that has parts cut off at its end.
 *
 * length represents the remaining length of the string;
 * functions using an object of this type SHALL NOT access memory locations beyond &literal[length].
 */
struct truncated_string_literal {
	const char *literal;
	size_t length;
};


/**
 * truncates a suffix from a string literal.
 *
 * raises 'false' if str doesn't end in the given suffix.
 */
constexpr truncated_string_literal get_prefix(const char *str, const char *suffix) {
	// if only c++14 had arrived a little bit earlier...

	return

	strlen(str) < strlen(suffix) ?
		// if suffix is longer than str
		throw false // string literal is shorter than supposed suffix
	: streq(str + (strlen(str) - strlen(suffix)), suffix) ?
		// elif str ends with suffix
		truncated_string_literal{str, strlen(str) - strlen(suffix)}
	:
		// else
		throw false; // string literal ends with wrong suffix
}


/**
 * creates a truncated_string_literal from a regular string literal
 */
constexpr truncated_string_literal create_truncated_string_literal(const char *str) {
	return truncated_string_literal{str, strlen(str)};
}

/**
 * tests whether a string literal starts with the given prefix
 *
 * @param pos  start checking at a certain position. for internal recursion usage only.
 */
constexpr bool has_prefix(const char *str, const truncated_string_literal prefix, size_t pos=0) {
	// if only c++14 had arrived a little bit earlier...

	return

	pos == prefix.length ?
		// if prefix was tested completely:
		true
	: str[pos] != prefix.literal[pos] ?
		// elif str differs from prefix:
		false
	:
		// else:
		has_prefix(str, prefix, pos + 1);
}

/**
 * strips a prefix from a given string literal.
 *
 * if the string literal doesn't have that prefix, returns the string literal itself.
 */
constexpr const char *strip_prefix(const char *str, const truncated_string_literal prefix) {
	// if only c++14 had arrived a little bit earlier...

	return

	has_prefix(str, prefix) ?
		// if str has the prefix:
		str + prefix.length
	:
		// else:
		str;
}


/**
 * strips a prefix, given as const char *, from a given string literal.
 *
 * if the string literal doesn't have that prefix, returns the string literal itself.
 */
constexpr const char *strip_prefix(const char *str, const char *prefix) {
	return strip_prefix(str, create_truncated_string_literal(prefix));
}

}}} // openage::util::constexpr_
