// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_set>
#include <vector>

#include "coord/scene.h"
#include "coord/tile.h"
#include "curve/discrete.h"
#include "time/time.h"
#include "util/vector.h"


namespace openage::renderer::terrain {

/**
 * Render entity for pushing updates to the Terrain renderer.
 */
class TerrainRenderEntity {
public:
	TerrainRenderEntity();
	~TerrainRenderEntity() = default;

	using terrain_elevation_t = util::FixedPoint<uint64_t, 16>;
	using tiles_t = std::vector<std::pair<terrain_elevation_t, std::string>>;

	/**
	 * Update a single tile of the displayed terrain (chunk) with information from the
	 * gamestate.
	 *
	 * @param size Size of the terrain in tiles (width x length)
	 * @param pos Position of the tile in the chunk.
	 * @param elevation Height of terrain tile.
	 * @param terrain_path Path to the terrain definition.
	 * @param time Simulation time of the update.
	 */
	void update_tile(const util::Vector2s size,
	                 const coord::tile &pos,
	                 const terrain_elevation_t elevation,
	                 const std::string terrain_path,
	                 const time::time_t time = 0.0);

	/**
	 * Update the full grid of the displayed terrain (chunk) with information from the
	 * gamestate.
	 *
	 * @param size Size of the terrain in tiles (width x length)
	 * @param tiles Animation data for each tile (elevation, terrain path).
	 * @param time Simulation time of the update.
	 */
	void update(const util::Vector2s size,
	            const tiles_t tiles,
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
	// const curve::Discrete<std::string> &get_terrain_path();

	/**
	 * Get the tiles of the terrain.
	 *
	 * @return Terrain tiles.
	 */
	const tiles_t &get_tiles();

	/**
	 * Get the terrain paths used in the terrain.
	 *
	 * @return Terrain paths.
	 */
	const std::unordered_set<std::string> &get_terrain_paths();

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
	 * Terrain tile information (elevation, terrain path).
	 */
	tiles_t tiles;

	/**
	 * Time of the last update call.
	 */
	time::time_t last_update;

	/**
	 * Terrain texture paths used in \p tiles .
	 */
	std::unordered_set<std::string> terrain_paths;

	/**
	 * Terrain vertices (ingame coordinates).
	 */
	std::vector<coord::scene3> vertices;

	/**
	 * Path to the terrain definition file.
	 */
	// curve::Discrete<std::string> terrain_path;

	/**
	 * Mutex for protecting threaded access.
	 */
	std::shared_mutex mutex;
};
} // namespace openage::renderer::terrain
