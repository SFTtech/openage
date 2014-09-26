#include <cstdio>
#include <cstdarg>

void throwerr(const char *fmt, ...) {
	// simple printf-style function that throws char * exceptions.
	va_list ap;
	va_start(ap, fmt);
	va_list aq;
	va_copy(aq, ap);
	size_t sz = vsnprintf(NULL, 0, fmt, aq) + 1;
	va_end(aq);
	char *result = new char[sz];
	vsnprintf(result, sz, fmt, ap);
	throw result;
	va_end(ap);
}
