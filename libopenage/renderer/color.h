// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>

namespace openage {
namespace renderer {

class Color {
	using color_channel_t = uint8_t;

public:
	Color();
	Color(color_channel_t r, color_channel_t g, color_channel_t b, color_channel_t a);

	Color(const Color &other) = default;
	Color(Color &&other) = default;
	Color &operator =(const Color &other) = default;
	Color &operator =(Color &&other) = default;

	bool operator ==(const Color &other) const;

	bool operator !=(const Color &other) const;

	color_channel_t r;
	color_channel_t g;
	color_channel_t b;
	color_channel_t a;
};

}} // openage::renderer
