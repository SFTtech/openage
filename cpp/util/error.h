#ifndef OPENAGE_UTIL_ERROR_H_6723759B2C7E4B9BA5F76C94A36B2033
#define OPENAGE_UTIL_ERROR_H_6723759B2C7E4B9BA5F76C94A36B2033

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

#endif //_UTIL_ERROR_H_
