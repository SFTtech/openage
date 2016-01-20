// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>

namespace openage {
namespace renderer {

class Color {
public:
	Color();

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	Color(const Color &other);

	Color &operator=(const Color &other);

	bool operator==(const Color &other) const;

	bool operator!=(const Color &other) const;

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

};

}} // openage::renderer
