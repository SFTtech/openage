// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_STRINGS_H_
#define OPENAGE_UTIL_STRINGS_H_

#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <functional>
#include <chrono>
#include <string>
#include <sstream>
#include <ios>
#include <iomanip>
#include <thread>
#include <memory>

namespace openage {
namespace util {

/**
 * formats fmt to a newly allocated memory area
 *
 * note that you need to manually free the returned result.
 */
char *format(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));

/**
 * same as format, but takes a va_list instead of ...
 */
char *vformat(const char *fmt, va_list ap);

/**
 * makes a copy of the string (up to and including the first null byte).
 *
 * note that you need to manually free both copies.
 */
char *copy(const char *s);

/**
 * removes all whitespace characters on the right of the string.
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
 * formats fmt_str to a std::string
 */
template <typename... Args>
std::string string_format(const char* fmt_str, Args&&... args) {
	// first guess of the actual string size	
	int final_n, n = ((int)strlen(fmt_str)) * 2; 
	// use of unique_ptr<char[]> as writing to std::string::c_str() would be unsafe.
	std::unique_ptr<char[]> formatted; 
	while(1) {
		formatted.reset(new char[n]); 
		final_n = snprintf(&formatted[0], n, fmt_str, std::forward<Args>(args)...);
		if (final_n < 0 || final_n >= n) {
			// First iteration: get the final formatted string size
			n += abs(final_n - n + 1);
		}
		else {
			// Second iteration: the string is now formatted and with and has a correct size
			break;
		}
	}
	// return the formatted string as a std::string
	return std::string(formatted.get());
}

/**
 * formats the timepoint in parameter to a std::string with the following format :
 * "hh:mm:ss.xxxxxxxx" of "aaaa-mm-jj hh:mm:ss.xxxxxxxx" if the show_date parameter is true
 */
template <typename TimePoint>
std::string timepoint_to_string(TimePoint const& timepoint, bool show_date=false) {
	auto seconds_since_epoch(std::chrono::duration_cast<std::chrono::seconds>(timepoint.time_since_epoch()));
	std::time_t now_t(std::chrono::system_clock::to_time_t(std::chrono::system_clock::time_point(seconds_since_epoch)));
	char temp[64] = {0};
	const char* fmt = (show_date ? "%F %H:%M:%S." : "%H:%M:%S.");
	if (!std::strftime(temp, sizeof(temp), fmt, std::localtime(&now_t))) {
		return "";
	}
	return std::string(temp) + std::to_string((timepoint.time_since_epoch() - seconds_since_epoch).count());
}
/**
 * formats the thread id in parameter to a std::string
 */
inline std::string threadid_to_string(std::thread::id thread_id) {
	std::stringstream ss;
	ss << "0x" << std::setw(8) << std::setfill('0') << std::hex << (std::hash<std::thread::id>{}(thread_id) & 0xffffffff);
	return ss.str();
}

} //namespace util
} //namespace openage

#endif
