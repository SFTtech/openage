// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>

namespace openage {
namespace renderer {

class Color {
	using color_channel_t = uint8_t;

public:
	Color();
	Color(color_channel_t r, color_channel_t g, color_channel_t b, color_channel_t a);

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	bool operator==(const Color &other) const;

	bool operator ==(const Color &other) const;

	bool operator !=(const Color &other) const;

	color_channel_t r;
	color_channel_t g;
	color_channel_t b;
	color_channel_t a;
};

}} // openage::renderer
