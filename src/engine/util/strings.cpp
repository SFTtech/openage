#include "strings.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace engine {
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

	char *result = new char[sz];

	vsnprintf(result, sz, fmt, ap);

	return result;
}

char *copy(const char *s) {
	size_t sz = strlen(s) + 1;

	char *result = new char[sz];
	memcpy(result, s, sz);

	return result;
}

size_t rstrip(char *s) {
	size_t strippedlen = strlen(s);

	while(strippedlen > 0) {
		if (
			s[strippedlen - 1] == '\n' or
			s[strippedlen - 1] == ' ' or
			s[strippedlen - 1] == '\t') {

			strippedlen -= 1;
		} else {
			break;
		}
	}

	s[strippedlen] = '\0';

	return strippedlen;
}

} //namespace util
} //namespace engine
