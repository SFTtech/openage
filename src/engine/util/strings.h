#ifndef _ENGINE_UTIL_STRINGS_H_
#define _ENGINE_UTIL_STRINGS_H_

#include <cstdlib>
#include <cstdarg>

namespace engine {
namespace util {

/**
formats fmt to a newly allocated memory area

note that you need to manually free the returned result.
*/
char *format(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));

/**
same as format, but takes a va_list instead of ...
*/
char *vformat(const char *fmt, va_list ap);

/**
makes a copy of the string (up to and including the first null byte).

note that you need to manually free both copies.
*/
char *copy(const char *s);

/**
removes all whitespace characters on the right of the string.
*/
size_t rstrip(char *s);

} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_STRINGS_H_
