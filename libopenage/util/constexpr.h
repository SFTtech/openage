// Copyright 2014-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdlib>


/**
 * This namespace contains constexpr functions, i.e. C++14 functions that are designed
 * to run at compile-time.
 */
namespace openage::util::constexpr_ {

/**
 * Returns true IFF the string literals have equal content.
 */
constexpr bool streq(const char *a, const char *b) {
	for (;*a == *b; ++a, ++b) {
		if (*a == '\0') {
			return true;
		}
	}
	return false;
}


/**
 * Returns the length of the string literal, excluding the terminating NULL byte.
 */
constexpr size_t strlen(const char *str) {
	for (size_t len = 0;; ++len) {
		if (str[len] == '\0') {
			return len;
		}
	}
}


/**
 * Stores a string literal plus a "length specifier".
 *
 * Due to the nature of C strings, parts can only be cut off at the start of the string;
 * this struct represents a string literal that has parts cut off at its end.
 *
 * length represents the remaining length of the string;
 * Functions using an object of this type SHALL NOT access memory locations beyond &literal[length].
 */
struct truncated_string_literal {
	const char *literal;
	size_t length;
};


/**
 * Truncates a suffix from a string literal.
 *
 * Raises 'false' if str doesn't end in the given suffix.
 */
constexpr truncated_string_literal get_prefix(const char *str, const char *suffix) {
	if (strlen(str) < strlen(suffix)) {
		// suffix is longer than str
		throw false;
	} else if (streq(str + (strlen(str) - strlen(suffix)), suffix)) {
		// str ends with suffix
		return truncated_string_literal{str, strlen(str) - strlen(suffix)};
	} else {
		throw false;
	}
}


/**
 * Creates a truncated_string_literal from a regular string literal.
 */
constexpr truncated_string_literal create_truncated_string_literal(const char *str) {
	return truncated_string_literal{str, strlen(str)};
}

/**
 * Tests whether a string literal starts with the given prefix.
 */
constexpr bool has_prefix(const char *str, const truncated_string_literal prefix) {
	for (size_t pos = 0; pos < prefix.length; ++pos) {
		if (str[pos] != prefix.literal[pos]) {
			return false;
		}
	}
	return true;
}


/**
 * Strips a prefix from a given string literal.
 *
 * If the string literal doesn't have that prefix, returns the string literal itself.
 */
constexpr const char *strip_prefix(const char *str, const truncated_string_literal prefix) {
	if (has_prefix(str, prefix)) {
		return str + prefix.length;
	} else {
		return str;
	}
}


/**
 * Strips a prefix, given as const char *, from a given string literal.
 *
 * If the string literal doesn't have that prefix, returns the string literal itself.
 */
constexpr const char *strip_prefix(const char *str, const char *prefix) {
	return strip_prefix(str, create_truncated_string_literal(prefix));
}

} // namespace openage::util::constexpr_
