// Copyright 2013-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>

#include <eigen3/Eigen/Dense>

namespace openage::renderer::resources {

/**
 * Describes a position of a (sub)texture within a larger texture.
 * Used for texture atlases and animations where all frames
 * are within one texture file.
 */
class Texture2dSubInfo {
public:
	/**
	 * Create a new 2D subtexture information.
	 *
	 * @param x Vertical position inside texture atlas.
	 * @param y Horizontal position inside texture atlas.
	 * @param w Width of subtexture.
	 * @param h Height of subtexture.
	 * @param cx Vertical anchor of subtexture.
	 * @param cy Horizontal anchor of subtexture.
     * @param atlas_width Width of the texture atlas containing the subtexture.
     * @param atlas_height Height of the texture atlas containing the subtexture.
	 */
	Texture2dSubInfo(uint32_t x,
	                 uint32_t y,
	                 uint32_t w,
	                 uint32_t h,
	                 uint32_t cx,
	                 uint32_t cy,
	                 uint32_t atlas_width,
	                 uint32_t atlas_height);

	Texture2dSubInfo() = default;

	/**
     * Get the normalized coordinates of the subtexture.
     *
     * Coordinates are in range (0.0, 1.0) and can be passed directly to a shader uniform.
     *
     * @return Pixel coordinates as 4-dimensional Eigen vector: (x, y, width, height)
     */
	const Eigen::Vector4f &get_tile_coords() const;

	/**
     * Get the normalized coordinates of the subtexture center.
     *
     * Coordinates are in range (0.0, 1.0) and can be passed directly to a shader uniform.
     *
     * @return Pixel coordinates as 2-dimensional Eigen vector: (x, y)
     */
	const Eigen::Vector2f &get_anchor_coords() const;

	/**
	 * Position within the atlas (top left corner).
	 */
	uint32_t x{}, y{};

	/**
	 * Size in pixels.
	 */
	uint32_t w{}, h{};

	/**
	 * Subtexture anchor within the atlas.
	 */
	uint32_t cx{}, cy{};

private:
	/**
     * Pre-computed normalized coordinates of the subtexture.
     */
	Eigen::Vector4f tile_coords;

	/**
     * Pre-computed normalized coordinates of the subtexture anchor.
     */
	Eigen::Vector2f anchor_coords;
};

} // namespace openage::renderer::resources
