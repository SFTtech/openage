// Copyright 2013-2024 the openage authors. See copying.md for legal info.

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
	 * Get the position of the subtexture anchor (origin == top left).
	 *
	 * @return Anchor coordinates as 2-dimensional Eigen vector: (x, y)
	 */
	const Eigen::Vector2i &get_anchor_pos() const;

	/**
	 * Get the normalized shader parameters of the subtexture. Use in the shader
	 * to sample the subtexture from the atlas.
	 *
	 * Values are in range (0.0, 1.0) and can be passed directly to a shader uniform.
	 * These parameters pre-computed and should be used whenever possible.
	 *
	 * @return Tile parameters as 4-dimensional Eigen vector: (x, y, width, height)
	 */
	const Eigen::Vector4f &get_subtex_coords() const;

	/**
	 * Get the anchor parameters of the subtexture center. Used in the shader
	 * to calculate the offset position for displaying the subtexture inside
	 * the OpenGL viewport.
	 *
	 * The parameters represent the pixel distance of the anchor point to the subtexture
	 * center, multiplied by 2 to account for the normalized viewport size (which is 2.0
	 * because it spans from -1.0 to 1.0).
	 *
	 * To get the normalized offset distance, the parameters have to be divided by the
	 * viewport size and then multiplied by additional scaling factors (e.g. from the
	 * animation).
	 *
	 * @return Parameters as 2-dimensional Eigen vector: (x, y)
	 */
	const Eigen::Vector2i &get_anchor_params() const;

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
	Eigen::Vector2i anchor_params;
};

} // namespace openage::renderer::resources
