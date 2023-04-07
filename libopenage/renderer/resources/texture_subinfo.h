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
	Texture2dSubInfo(int32_t x,
	                 int32_t y,
	                 uint32_t w,
	                 uint32_t h,
	                 int32_t cx,
	                 int32_t cy,
	                 uint32_t atlas_width,
	                 uint32_t atlas_height);

	Texture2dSubInfo() = default;

	/**
     * Get the position of the subtexture within the atlas.
     *
     * @return Pixel coordinates as 2-dimensional Eigen vector: (x, y)
     */
	const Eigen::Vector2i &get_pos() const;

	/**
     * Get the size of the subtexture.
     *
     * @return Pixel coordinates as 2-dimensional Eigen vector: (width, height)
     */
	const Eigen::Vector2<uint32_t> &get_size() const;

	/**
     * Get the position of the subtexture anchor.
     *
     * @return Anchor coordinates as 2-dimensional Eigen vector: (x, y)
     */
	const Eigen::Vector2i &get_anchor_pos() const;

	/**
     * Get the normalized shader parameters of the subtexture.
     *
     * Coordinates are in range (0.0, 1.0) and can be passed directly to a shader uniform.
     *
     * @return Tile parameters as 4-dimensional Eigen vector: (x, y, width, height)
     */
	const Eigen::Vector4f &get_tile_params() const;

	/**
     * Get the normalized shader parameters of the subtexture center.
     *
     * Coordinates are in range (0.0, 1.0) and can be passed directly to a shader uniform.
     *
     * @return Pixel coordinates as 2-dimensional Eigen vector: (x, y)
     */
	const Eigen::Vector2f &get_anchor_params() const;

private:
	/**
	 * Position within the atlas (top left corner: x, y).
	 */
	Eigen::Vector2i pos;

	/**
	 * Size in pixels (width, height).
	 */
	Eigen::Vector2<uint32_t> size;

	/**
	 * Subtexture anchor relative to the subtexture's top left (x, y).
	 */
	Eigen::Vector2i anchor_pos;

	/**
     * Pre-computed normalized coordinates of the subtexture.
     */
	Eigen::Vector4f tile_params;

	/**
     * Pre-computed normalized coordinates of the subtexture anchor.
     */
	Eigen::Vector2f anchor_params;
};

} // namespace openage::renderer::resources
