#include "log.h"

#include <cstdio>
#include <cstdarg>

namespace openage {
namespace log {

constexpr LogLevel loglevels[] {
	{"FATAL", "31;1"},
	{"ERR  ", "31"},
	{"WARN ", "34"},
	{"IMP  ", "38"},
	{"MSG  ", ""},
	{"DBG2 ", ""},
	{"DBG1 ", ""},
	{"DBG0 ", ""}
};

template <int lvl>
void log(const char *fmt, ...) {
	const char *name = loglevels[lvl].name;
	const char *col = loglevels[lvl].col;

	printf("\x1b[%sm%s (%d)\x1b[m: ", col, name, lvl);

	va_list vl;
	va_start(vl, fmt);
	vprintf(fmt, vl);
	va_end(vl);
}

} //namespace log
} //namespace openage
