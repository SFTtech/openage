// Copyright 2013-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdarg>
#include <functional>
#include <vector>
#include <iomanip>


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
 * tokenizes str by splitting it up to substrings at the deliminiters.
 * this is done by replacing the deliminiters by '\0' in the original string.
 * '\n' is evaluated correctly; all other '\x' escape sequences are evaluated
 * to literal x, including the deliminiter.
 *
 * the number of tokens is returned, but only bufsize tokens are written to buf.
 */
size_t string_tokenize_to_buf(char *str, char delim, char **buf, size_t bufsize);


/**
 * behaviour is as with string_tokenize_to_buf, but the result buffer is
 * dynamically allocated to match the token count.
 * you need to manually free it using delete[].
 *
 * returns the number of tokens read.
 * result[retval] == nullptr.
 */
size_t string_tokenize_dynamic(char *str, char delim, char ***result);


/**
 * base tokenizer method, which is used internally by string_tokenize_to_buf
 * and string_tokenize_dynamic.
 * calls callback each time a new token was found. note that at the point where
 * callback is called, the token content itself is not yet processed.
 */
void string_tokenize_base(char *str, char delim, std::function<void(char *)> callback);


/**
 * returns true if str matches the basic globbing pattern
 * in the pattern, '*' matches any number of characters, while all other
 * characters are interpreted as literal.
 */
bool string_matches_pattern(const char *str, const char *pattern);


}} // openage::util
