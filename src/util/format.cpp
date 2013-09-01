#include "format.h"

#include <cstdlib>
#include <cstdio>

namespace openage {
namespace util {

char *format(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	char *result = vformat(fmt, ap);

	va_end(ap);

	return result;
}

char *vformat(const char *fmt, va_list ap) {
	va_list aq;
	va_copy(aq, ap);

	size_t sz = vsnprintf(NULL, 0, fmt, aq) + 1;
	va_end(aq);

	char *result = (char *) malloc(sz);

	vsnprintf(result, sz, fmt, ap);

	return result;
}

} //namespace util
} //namespace openage
