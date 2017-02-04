// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "strings.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "../config.h"
#include "compiler.h"

namespace openage {
namespace util {


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


void copy_string(const char *s, std::vector<char> target) {
	size_t sz = strlen(s) + 1;
	target.resize(sz);
	memcpy(target.data(), s, sz);
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


void string_tokenize_base(char *str, char delim, std::function<void(char *)> callback) {
	callback(str);

	char *r = str;
	//the output buf is the input buf. w <= r is guaranteed.
	//(r - w) will increment each time an escape sequence is read.
	char *w = str;

	while (true) {
		if (*r == '\0') {
			//we have read the end of the input string
			//append '\0' to output string
			*(w++) = '\0';

			break;
		}
		if (*r == delim) {
			//we have read a deliminiter
			//write a '\0', and store the pointer to
			//the next token (the char after the '\0')

			*(w++) = '\0';

			callback(w);

			r++;

			continue;
		}
		if (*r == '\\') {
			//an escaped char: increment the read pointer to point
			//at the escape code.
			r++;

			//analyze the escape code
			switch (*r) {
			case '\0':
				//string ended in the middle of an escape code
				//error
				return;
				break;
			case 'n':
				//a newline
				*r = '\n';
				break;
			default:
				//the escape code already represents the literal
				//character (e.g.: '\\', '\,').
				break;
			}

			//'fall through' to the default action
		}

		//copy current char to output buf
		*(w++) = *(r++);
	}
}


size_t string_tokenize_to_buf(char *str, char delim, char **buf, size_t bufsize) {
	size_t count = 0;

	//my first lambda!
	std::function<void(char *)> callback = [&buf, bufsize, &count] (char *arg) -> void {
		if (count < bufsize) {
			buf[count] = arg;
		}
		count++;
	};

	string_tokenize_base(str, delim, callback);

	return count;
}


size_t string_tokenize_dynamic(char *str, char delim, char ***result) {
	std::vector<char *> resultvector;

	//my second lambda! (actually not so exciting anymore now.)
	std::function<void(char *)> callback = [&resultvector] (char *arg) -> void {
		resultvector.push_back(arg);
	};

	string_tokenize_base(str, delim, callback);

	*result = new char *[resultvector.size() + 1];
	for (size_t i = 0; i < resultvector.size(); i++) {
		(*result)[i] = resultvector[i];
	}
	(*result)[resultvector.size()] = nullptr;

	return resultvector.size();
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
	split(txt, delimiter, std::back_inserter(items));
	return items;
}


}} // openage::util
