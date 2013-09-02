#include "error.h"

#include <cstdarg>
#include <cstdlib>
#include <cstring>

#include "strings.h"
#include "../log/log.h"

namespace openage {
namespace util {

Error::Error(const char *fmt, ...) {
	va_list vl;
	va_start(vl, fmt);
	this->buf = vformat(fmt, vl);
	va_end(vl);
}

Error::Error(Error const &other) {
	buf = copy(other.buf);
}

Error &Error::operator=(Error const &other) {
	free(buf);
	buf = copy(other.buf);
	return *this;
}

Error::Error(Error &&other) {
	buf = other.buf;
	other.buf = NULL;
}


Error::~Error() {
	free((void *) buf);
}

const char *Error::str() {
	return buf;
}

} //namespace util
} //namespace openage
