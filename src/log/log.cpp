#include "log.h"

#include <cstdio>
#include <cstdarg>

namespace openage {
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

	printf("\x1b[%sm%s\x1b[m ", col, name);

	va_list vl;
	va_start(vl, fmt);
	vprintf(fmt, vl);
	va_end(vl);

	printf("\n");

	//TODO: store the log message into an array, which can then be read by the engine, for rendering the log text as an overlay
}

//this function exists purely to make the compiler
//generate the templated functions... TODO: solve it without this
//dirty hack...
void log_test() {
	log<0>("lol");
	log<1>("this");
	log<2>("is");
	log<3>("a");
	log<4>("stupid");
	log<5>("logger");
	log<6>("test");
	log<7>("function");
	log<8>("call");
	log<9>("!!");
}

} //namespace log
} //namespace openage
