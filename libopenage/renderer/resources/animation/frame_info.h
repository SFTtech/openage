// Copyright 2021-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>

namespace openage::renderer::resources {

/**
 * Contains information about a 2D animation frame. The frame image data
 * is stored in a 2D texture object.
 */
class FrameInfo {
public:
	/**
	 * Create a 2D Frame Info.
	 *
	 * @param texture_idx Index of the texture containing the frame in the
	 *                    Animation2dInfo object.
	 * @param subtexture_idx Index of the subtexture containing the frame
	 *                    in the Texture2dInfo object.
	 */
	FrameInfo(const size_t texture_idx,
	          const size_t subtexture_idx);

	FrameInfo() = default;
	~FrameInfo() = default;

	/**
	 * Get the texture index of the frame in the animation.
	 *
	 * @return Index of a texture in a Animation2dInfo object.
	 */
	size_t get_texture_idx() const;

	/**
	 * Get the subtexture index of the frame in the texture.
	 *
	 * @return Index of a subtexture in a Texture2dInfo object.
	 */
	size_t get_subtexture_idx() const;

private:
	/**
	 * Index of the texture containing the frame in the Animation2dInfo object.
	 */
	size_t texture_idx;

	/**
	 * Index of the subtexture containing the frame in the Texture2dInfo object.
	 */
	size_t subtexture_idx;
};

} // namespace openage::renderer::resources
