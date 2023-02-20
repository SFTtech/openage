// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "palette_info.h"

namespace openage::renderer::resources {

PaletteInfo::PaletteInfo(const std::vector<uint8_t> &colors) :
	colors{} {
	for (size_t i = 0; i < colors.size(); i += 4) {
		this->colors.emplace_back(colors[i] / 256.0,
		                          colors[i + 1] / 256.0,
		                          colors[i + 2] / 256.0,
		                          colors[i + 3] / 256.0);
	}
}

PaletteInfo::PaletteInfo(const std::vector<Eigen::Vector4f> &colors) :
	colors{colors} {}

Eigen::Vector4f PaletteInfo::get_color(size_t idx) {
	return this->colors[idx];
}

const std::vector<Eigen::Vector4f> PaletteInfo::get_colors() {
	return this->colors;
}

} // namespace openage::renderer::resources
