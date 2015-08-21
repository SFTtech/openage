// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "color.h"

namespace openage {
namespace renderer {

Color::Color()
	:
	r{0},
	g{0},
	b{0},
	a{255} {
	// Empty
}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	:
	r{r},
	g{g},
	b{b},
	a{a} {
	// Empty
}

Color::Color(const Color &other)
	:
	r{other.r},
	g{other.g},
	b{other.b},
	a{other.a} {
	// Empty
}

Color &Color::operator=(const Color &other) {
	if (this != &other) {
		this->r = other.r;
		this->g = other.g;
		this->b = other.b;
		this->a = other.a;
	}

	return *this;
}

bool operator==(const Color &left, const Color &right) {
	return left.r == right.r && left.g == right.g && left.b == right.b && left.a == right.a;
}

bool operator!=(const Color &left, const Color &right) {
	return !(left == right);
}

}} // openage::renderer
