// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_RENDERER_COLOR_H_
#define OPENAGE_RENDERER_COLOR_H_

#include <cstdint>

namespace openage {
namespace renderer {

class Color {
public:
	Color();

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	Color(const Color &other);

	Color &operator=(const Color &other);

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

};

bool operator==(const Color &left, const Color &right);
bool operator!=(const Color &left, const Color &right);

}} // openage::renderer

#endif
