#ifndef _UTIL_ERROR_H_
#define _UTIL_ERROR_H_

#include <cstdarg>
#include <cstdio>

namespace openage {
namespace util {

/**
Exception type
*/
class Error {
public:
	Error(const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
	~Error();

	const char *str();

private:
	const char *buf;
};

} //namespace util
} //namespace openage

#endif //_UTIL_ERROR_H_
