// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "util/path.h"
#include "util/vector.h"

namespace openage::renderer {
// ASDF: testing
class Renderer;

namespace terrain {

struct TerrainVertex {
	float x;
	float y;
	float height;
};

class TerrainRenderEntity {
public:
	TerrainRenderEntity(
		// const std::shared_ptr<renderer::Renderer> &renderer,
		// const util::Path &assetdir
	);
	~TerrainRenderEntity() = default;

	/**
     * Update the render entity with information from the
     * gamestate.
	 * 
	 * @param size Size of the terrain in tiles (width x length)
	 * @param height_map Height of terrain tiles.
	 * @param texture_path Path to the terrain texture.
     */
	void update(util::Vector2s size,
	            std::vector<float> height_map,
	            const util::Path &texture_path);

	/**
     * Get the vertices of the terrain.
     *
     * @return Vector of vertex coordinates.
     */
	const std::vector<TerrainVertex> &get_vertices();

	/**
     * Get the texture mapping for the terrain.
     *
     * TODO: Return the actual mapping.
     *
     * @return Texture mapping of textures to vertex area.
     */
	const util::Path &get_texture_path();

	/**
     * Get the number of vertices on each side of the terrain.
     *
     * @return Vector with width as first element and height as second element.
     */
	const util::Vector2s &get_size();

	/**
	 * Check whether the render entity has received new updates from the
	 * gamestate.
	 * 
	 * @return true if updates have been received, else false.
	 */
	bool is_changed();

	/**
	 * Clear the update flag by setting it to false.
	 */
	void clear_changed_flag();

private:
	/**
	 * Flag for determining if the render entity has been updated by the
	 * corresponding gamestate entity. Set to true every time \p update()
	 * is called.
	 */
	bool changed;

	/**
     * Terrain dimensions (width x height).
     */
	util::Vector2s size;

	std::vector<TerrainVertex> vertices;
	util::Path texture_path;
	// std::unordered_map<Texture2d, size_t> texture_map; // texture -> vertex indices
};
} // namespace terrain
} // namespace openage::renderer