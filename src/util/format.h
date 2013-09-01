#ifndef _UTIL_FORMAT_H_
#define _UTIL_FORMAT_H_

#include <cstdarg>

namespace openage {
namespace util {

char *format(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
char *vformat(const char *fmt, va_list ap);

} //namespace util
} //namespace openage

#endif //_UTIL_FORMAT_H_
