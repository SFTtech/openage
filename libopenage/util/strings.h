// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdarg>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>


namespace openage {
namespace util {


/**
 * Quick-formatter for floats when working with string streams.
 * Usage: cout << FormatFloat{1.0, 10};
 */
template<unsigned decimals, unsigned w=0>
struct FloatFixed {
	float value;
};


template<unsigned decimals, unsigned w>
std::ostream &operator <<(std::ostream &os, FloatFixed<decimals, w> f) {
	static_assert(decimals < 50, "Refusing to print float with >= 50 decimals");
	static_assert(w < 70, "Refusing to print float with a width >= 70");

	os.precision(decimals);
	os << std::fixed;

	if (w) {
		os << std::setw(w);
	}

	os << f.value;

	return os;
}


template<unsigned divisor, unsigned decimals=3, unsigned w=0>
struct FixedPoint {
	int64_t value;
};


template<unsigned divisor, unsigned decimals, unsigned w>
std::ostream &operator <<(std::ostream &os, FixedPoint<divisor, decimals, w> f) {
	static_assert(divisor > 0, "Divisor for fixed-point numbers must be > 0");

	os << FloatFixed<decimals, w>{((float) f.value) / (float) divisor};
	return os;
}

/**
 * printf-style to-string formatting.
 */
std::string sformat(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));


/**
 * printf-style valist-to-string formatting; the result is appended to output.
 */
size_t vsformat(const char *fmt, va_list ap, std::string &output);


/**
 * Copies the given string to the vector target.
 * Target is resized to exactly hold the data (including the NULL byte).
 */
void copy_string(const char *s, std::vector<char> target);


/**
 * Returns the number of whitespace characters on the right of the string.
 */
size_t rstrip(char *s);


/**
 * returns true if str matches the basic globbing pattern
 * in the pattern, '*' matches any number of characters, while all other
 * characters are interpreted as literal.
 */
bool string_matches_pattern(const char *str, const char *pattern);


/**
 * Split a string at a delimiter, push the result back in an iterator.
 * Why doesn't the fucking standard library have std::string::split(delimiter)?
 */
template<typename ret_t>
void split(const std::string &txt, char delimiter, ret_t result) {
	std::stringstream splitter;
	splitter.str(txt);
	std::string part;

	while (std::getline(splitter, part, delimiter)) {
		*result = part;
		result++;
	}
}


/**
 * Split a string at a delimiter into a vector.
 * Internally, uses the above iterator splitter.
 */
std::vector<std::string> split(const std::string &txt, char delim);


/**
 * Split a string at a delimiter into a vector.
 * size_hint is to give a predicted size of the vector already.
 *
 * tokenizes txt by splitting it up to substrings at the deliminiters.
 * "\n" is evaluated to '\n'; all other '\X' escape sequences are evaluated
 * to literal X, including the deliminiter.
 */
std::vector<std::string> split_escape(const std::string &txt,
                                      char delim, size_t size_hint=0);

}} // openage::util
