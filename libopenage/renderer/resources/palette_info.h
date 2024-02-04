// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <eigen3/Eigen/Dense>


namespace openage::renderer::resources {

/**
 * Contains information about a color palette.
 */
class PaletteInfo {
public:
	/**
	 * Create a color palette.
	 *
	 * @param colors Sequence of RGBA values. Size must be a multiple of 4.
	 */
	PaletteInfo(const std::vector<uint8_t> &colors);

	/**
	 * Create a color palette.
	 *
	 * @param colors List of normalized RGBA colors.
	 */
	PaletteInfo(const std::vector<Eigen::Vector4f> &colors);

	PaletteInfo() = default;
	~PaletteInfo() = default;

	/**
	 * Get a color in the palette with a specified index.
	 *
	 * @return Normalized RGBA color vector.
	 */
	Eigen::Vector4f get_color(size_t idx);

	/**
	 * Get the colors of the palette.
	 *
	 * @return List of normalized RGBA colors.
	 */
	const std::vector<Eigen::Vector4f> get_colors();

private:
	/**
	 * Colors inside the palette.
	 */
	std::vector<Eigen::Vector4f> colors;
};

} // namespace openage::renderer::resources
