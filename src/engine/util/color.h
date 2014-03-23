#ifndef _ENGINE_UTIL_COLOR_H_
#define _ENGINE_UTIL_COLOR_H_

#include "../../gamedata/color.h"

namespace engine {
namespace util {

struct col {
	col(unsigned r, unsigned g, unsigned b, unsigned a) : r{r}, g{g}, b{b}, a{a} {}
	col(palette_color c);

	unsigned r, g, b, a;

	void use();
	void use(float alpha);
};

} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_COLOR_H_
