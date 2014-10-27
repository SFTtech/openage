#ifndef OPENAGE_UTIL_COLOR_H_436CD7F56DC14715A9D58B748BEB21DE
#define OPENAGE_UTIL_COLOR_H_436CD7F56DC14715A9D58B748BEB21DE

#include "../gamedata/color.gen.h"

namespace openage {
namespace util {

struct col {
	col(unsigned r, unsigned g, unsigned b, unsigned a) : r{r}, g{g}, b{b}, a{a} {}
	col(gamedata::palette_color c);

	unsigned r, g, b, a;

	void use();
	void use(float alpha);
};

} //namespace util
} //namespace openage

#endif //_UTIL_COLOR_H_
