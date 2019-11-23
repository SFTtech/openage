// Copyright 2013-2019 the openage authors. See copying.md for legal info.

#include "strings.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "config.h"
#include "../error/error.h"
#include "compiler.h"

namespace openage::util {


std::string sformat(const char *fmt, ...) {
	std::string ret;

	va_list ap;
	va_start(ap, fmt);
	vsformat(fmt, ap, ret);
	va_end(ap);

	return ret;
}


size_t vsformat(const char *fmt, va_list ap, std::string &output) {
#if HAVE_THREAD_LOCAL_STORAGE
	static thread_local std::vector<char> buf(64);
#else
	std::vector<char> buf(64);
#endif

	va_list aq;
	va_copy(aq, ap);
	size_t length = vsnprintf(buf.data(), buf.size(), fmt, aq);
	va_end(aq);

	if (unlikely(length >= buf.size())) {
		size_t target_size = buf.size();
		if (unlikely(target_size < 64)) {
			target_size = 64;
		}
		while (length >= target_size) {
			target_size *= 2;
		}

		buf.resize(target_size);

		vsnprintf(buf.data(), buf.size(), fmt, ap);
	}

	output.append(buf.data(), length);

	return length;
}


std::unique_ptr<char[]> copy_string(const char *s) {
	size_t sz = strlen(s) + 1;
	auto ret = std::make_unique<char[]>(sz);
	memcpy(ret.get(), s, sz);
	return ret;
}


size_t rstrip(char *s) {
	size_t strippedlen = strlen(s);

	while (strippedlen > 0) {
		if (s[strippedlen - 1] == '\n' ||
		    s[strippedlen - 1] == ' '  ||
		    s[strippedlen - 1] == '\t') {

			strippedlen -= 1;
		} else {
			break;
		}
	}

	s[strippedlen] = '\0';

	return strippedlen;
}


bool string_matches_pattern(const char *str, const char *pattern) {
	while (true) {
		if (*pattern == '*') {
			// skip all wildcard chars
			while (*pattern == '*') {
				pattern++;
			}

			// performance optimization: if the wildcard was the
			// last char of the pattern, it's a sure match.
			if (*pattern == '\0') {
				return true;
			}

			// search for all places in str that equal *pattern;
			// those are possible places of continuation.
			while (*str != '\0') {
				if (*str == *pattern) {
					if (string_matches_pattern(str, pattern)) {
						return true;
					}
				}
			}

			// no match was found
			return false;
		}

		if (*str != *pattern) {
			// chars don't match
			return false;
		}

		if (*pattern == '\0') {
			// comparision done
			return true;
		}

		str += 1;
		pattern += 1;
	}
}


std::vector<std::string> split(const std::string &txt, char delimiter) {
	std::vector<std::string> items;
	// use the back inserter iterator and the templated split function.
	split(txt, delimiter, std::back_inserter(items));
	return items;
}


std::vector<std::string> split_escape(const std::string &txt, char delim, size_t size_hint) {

	// output vector
	std::vector<std::string> items;
	if (likely(size_hint)) {
		items.reserve(size_hint);
	}

#if HAVE_THREAD_LOCAL_STORAGE
	static thread_local std::vector<char> buf;
#else
	std::vector<char> buf;
	buf.reserve(256);
#endif

	// string reading pointer
	const char *r = txt.c_str();

	// copy characters to buf, and a buf is emitted as a token
	// when the delimiter or end is reached.
	while (true) {

		// end of input string
		if (*r == '\0') {
			items.emplace_back(std::begin(buf), std::end(buf));
			buf.clear();
			break;
		}

		// delimiter found
		if (*r == delim) {
			items.emplace_back(std::begin(buf), std::end(buf));
			buf.clear();

			r++;

			continue;
		}

		if (*r == '\\') {
			// an escaped char: increment the read pointer to point
			// at the escape code.
			r++;

			// analyze the escape code
			switch (*r) {
			case '\0':
				// string ended in the middle of an escape code
				// error!
				throw Error{ERR << "string ends after escape"};

			case 'n':
				// a newline
				buf.push_back('\n');
				continue;

			default:
				// the escape code already represents the literal
				// character (e.g.: "\\" = '\', "\," = ',').
				break;
			}
		}

		buf.push_back(*r);
		r++;
	}

	return items;
}

} // openage::util
