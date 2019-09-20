// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>

namespace openage {
namespace renderer {

class Color {
public:

	Color();

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	bool operator==(const Color &other) const;

	bool operator!=(const Color &other) const;

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

};

class Colors {
public:

	static Color WHITE;
	static Color BLACK;

};

}} // openage::renderer
