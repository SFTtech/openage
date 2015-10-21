// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "color.h"

namespace openage {
namespace renderer {

Color::Color()
	:
	r{0},
	g{0},
	b{0},
	a{255} {}

Color::Color(color_channel_t r, color_channel_t g,
             color_channel_t b, color_channel_t a)
	:
	r{r},
	g{g},
	b{b},
	a{a} {}

bool Color::operator ==(const Color &other) const {
	return (this->r == other.r &&
	        this->g == other.g &&
	        this->b == other.b &&
	        this->a == other.a);
}

bool Color::operator !=(const Color &other) const {
	return not (*this == other);
}

}} // openage::renderer
