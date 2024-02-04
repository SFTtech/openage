// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <optional>

namespace openage::renderer::resources {

/**
 * Contains information about a 2D terrain frame. The frame image data
 * is stored in a 2D texture object.
 */
class TerrainFrameInfo {
public:
	/**
	 * Create a 2D Frame Info.
	 *
	 * @param texture_idx Index of the texture containing the frame in the
	 *                    TerrainInfo object.
	 * @param subtexture_idx Index of the subtexture containing the frame
	 *                       in the Texture2dInfo object.
	 * @param priority Blending priority.
	 * @param priority Blend mode index.
	 */
	TerrainFrameInfo(size_t texture_idx,
	                 size_t subtexture_idx,
	                 size_t priority = 0,
	                 std::optional<size_t> blend_mode = std::nullopt);

	TerrainFrameInfo() = default;
	~TerrainFrameInfo() = default;

	/**
	 * Get the texture index of the frame in the animation.
	 *
	 * @return Index of a texture in a TerrainInfo object.
	 */
	size_t get_texture_idx() const;

	/**
	 * Get the subtexture index of the frame in the texture.
	 *
	 * @return Index of a subtexture in a Texture2dInfo object.
	 */
	size_t get_subtexture_idx() const;

	/**
	 * Get the blending priority of the frame.
	 *
	 * @return Blending priority.
	 */
	size_t get_priority() const;

	/**
	 * Get the blending mode of the frame.
	 *
	 * @return Blending mode.
	 */
	std::optional<size_t> get_blend_mode() const;

private:
	/**
	 * Index of the texture containing the frame in the TerrainInfo object.
	 */
	size_t texture_idx;

	/**
	 * Index of the subtexture containing the frame in the Texture2dInfo object.
	 */
	size_t subtexture_idx;

	/**
	 * Decides which blending table of the two adjacent terrain textures is selected.
	 *
	 * If two adjacent terrains have equal priority, the blending table of the terrain
	 * with a lower x coordinate value is selected. If the x coordinate value is also
	 * equal, the blending table of the terrain with the lowest y coordinate is selected.
	 */
	size_t priority;

	/**
	 * Used for looking up the blending pattern index in a blending table.
	 */
	std::optional<size_t> blend_mode;
};

} // namespace openage::renderer::resources
