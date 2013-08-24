#ifndef _LOG_LOG_H_
#define _LOG_LOG_H_

#include <cstdio>
#include <cstdarg>

namespace openage {
namespace log {

struct LogLevel {
	const char *name;
	const char *col;
};

template <int lvl>
void log(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));

using logfunction_ptr = void (*) (const char *, ...);

constexpr logfunction_ptr fatal = log<0>;
constexpr logfunction_ptr err = log<1>;
constexpr logfunction_ptr warn = log<2>;
constexpr logfunction_ptr imp = log<3>;
constexpr logfunction_ptr msg = log<4>;
constexpr logfunction_ptr dbg2 = log<5>;
constexpr logfunction_ptr dbg1 = log<6>;
constexpr logfunction_ptr dbg0 = log<7>;
constexpr logfunction_ptr dbg = log<7>;

} //namespace log
} //namespace openage

#endif //_LOG_LOG_H_
