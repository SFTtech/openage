#ifndef OPENAGE_UTIL_COLOR_H_
#define OPENAGE_UTIL_COLOR_H_

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

#endif
