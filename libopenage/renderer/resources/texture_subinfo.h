// Copyright 2013-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>


namespace openage::renderer::resources {

/**
 * Describes a position of a (sub)texture within a larger texture.
 * Used for texture atlases and animations where all frames
 * are within one texture file.
 */
struct Texture2dSubInfo {
	/**
	 * Position within the atlas (top left corner).
	 */
	uint32_t x{}, y{};

	/**
	 * Size in pixels.
	 */
	uint32_t w{}, h{};

	/**
	 * Subtexture center within the atlas.
	 */
	uint32_t cx{}, cy{};

	/**
	 * Create a new 2D subtexture information.
	 *
	 * @param x Vertical position inside texture atlas.
	 * @param y Horizontal position inside texture atlas.
	 * @param w Width of subtexture.
	 * @param h Height of subtexture.
	 * @param cx Vertical center of subtexture.
	 * @param cy Horizontal center of subtexture.
	 */
	Texture2dSubInfo(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t cx, uint32_t cy);

	Texture2dSubInfo() = default;
};

} // namespace openage::renderer::resources
