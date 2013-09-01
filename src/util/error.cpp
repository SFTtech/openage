#include "error.h"

#include <cstdarg>
#include <cstdlib>

#include "format.h"

namespace openage {
namespace util {

Error::Error(const char *fmt, ...) {
	va_list vl;
	va_start(vl, fmt);
	buf = vformat(fmt, vl);
	va_end(vl);
}

Error::~Error() {
	free((void *) buf);
}

const char *Error::str() {
	return buf;
}

} //namespace util
} //namespace openage
