// Copyright 2013+ the openage authors. See copying.md for legal info.


module;

#include <epoxy/gl.h>

export module openage.util.color;

export namespace openage::util {

struct col {
	col(unsigned r, unsigned g, unsigned b, unsigned a) :
		r{r}, g{g}, b{b}, a{a} {}

	unsigned r, g, b, a;

	void use();
	void use(float alpha);
};

} // namespace openage::util

namespace openage::util {

void col::use() {
	//TODO use glColor4b
	glColor4f(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

void col::use(float alpha) {
	glColor4f(r / 255.f, g / 255.f, b / 255.f, alpha);
}

} // namespace openage::util
