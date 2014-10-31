// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_STRINGS_H_
#define OPENAGE_UTIL_STRINGS_H_

#include <cstdarg>
#include <cstdlib>
#include <functional>

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

} //namespace util
} //namespace openage

#endif
