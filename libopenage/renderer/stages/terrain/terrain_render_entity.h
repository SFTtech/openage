// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>

#include "coord/scene.h"
#include "curve/discrete.h"
#include "time/time.h"
#include "util/vector.h"


namespace openage::renderer {

namespace terrain {

class TerrainRenderEntity {
public:
	TerrainRenderEntity();
	~TerrainRenderEntity() = default;

	/**
     * Update the render entity with information from the
     * gamestate.
	 *
	 * @param size Size of the terrain in tiles (width x length)
	 * @param height_map Height of terrain tiles.
	 * @param terrain_path Path to the terrain definition.
     * @param time Simulation time of the update.
     */
	void update(util::Vector2s size,
	            std::vector<float> height_map,
	            const std::string terrain_path,
	            const time::time_t time = 0.0);

	/**
     * Get the vertices of the terrain.
     *
     * @return Vector of vertex coordinates.
     */
	const std::vector<coord::scene3> &get_vertices();

	/**
     * Get the texture mapping for the terrain.
     *
     * TODO: Return the actual mapping.
     *
     * @return Texture mapping of textures to vertex area.
     */
	const curve::Discrete<std::string> &get_terrain_path();

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
     * Chunk dimensions (width x height).
     */
	util::Vector2s size;

	/**
	 * Terrain vertices (ingame coordinates).
	 */
	std::vector<coord::scene3> vertices;

	/**
	 * Path to the terrain definition file.
	 */
	curve::Discrete<std::string> terrain_path;

	// std::unordered_map<Texture2d, size_t> texture_map; // texture -> vertex indices

	/**
	 * Mutex for protecting threaded access.
	 */
	std::shared_mutex mutex;
};
} // namespace terrain
} // namespace openage::renderer
