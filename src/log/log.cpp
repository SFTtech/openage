#include "log.h"

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>

#include "../util/strings.h"

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

	char *tmp0 = util::format("\x1b[%sm%s\x1b[m", col, name);

	va_list ap;
	va_start(ap, fmt);
	char *tmp1 = util::vformat(fmt, ap);
	va_end(ap);

	char *buf = util::format("%s %s", tmp0, tmp1);

	delete[] tmp0;
	delete[] tmp1;

	util::rstrip(buf);

	puts(buf);

	//TODO: store buf into an array, which can then be evaluated and displayed by the engine

	delete[] buf;
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
