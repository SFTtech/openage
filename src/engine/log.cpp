#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "util/strings.h"
#include "engine.h"
#include "console/console.h"

namespace engine {
namespace log {

constexpr LogLevel loglevels[] {
	{"FATAL", "31;1"},
	{"ERROR", "31"},
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

	char *tmp0 = util::format("\x1b[%sm%s\x1b[m", col, name);

	va_list ap;
	va_start(ap, fmt);
	char *tmp1 = util::vformat(fmt, ap);
	va_end(ap);

	char *buf = util::format("%s %s", tmp0, tmp1);

	delete[] tmp0;
	delete[] tmp1;

	util::rstrip(buf);

	//TODO use more general callback functions for printing log messages

	puts(buf);

	if (engine::running) {
		engine::console::write(buf);
	}

	delete[] buf;
}

//force instantiation of the log functions
template void log<0>(const char *fmt, ...);
template void log<1>(const char *fmt, ...);
template void log<2>(const char *fmt, ...);
template void log<3>(const char *fmt, ...);
template void log<4>(const char *fmt, ...);
template void log<5>(const char *fmt, ...);
template void log<6>(const char *fmt, ...);
template void log<7>(const char *fmt, ...);

} //namespace log
} //namespace engine
