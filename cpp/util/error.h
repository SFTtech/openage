#ifndef OPENAGE_UTIL_ERROR_H_
#define OPENAGE_UTIL_ERROR_H_

#include <cstdarg>
#include <cstdio>

namespace openage {
namespace util {

/**
Exception type
*/
class Error {
public:
	/**
	constructor
	*/
	Error(const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

	/**
	copy constructor
	*/
	Error(Error const &copy);

	/**
	assignment op
	*/
	Error &operator=(Error const &copy);

	/**
	move constructor
	*/
	Error(Error &&other);

	~Error();

	const char *str();

private:
	char *buf;
};

} //namespace util
} //namespace openage

#endif
