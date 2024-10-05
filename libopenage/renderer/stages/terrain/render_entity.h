// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "coord/scene.h"
#include "coord/tile.h"
#include "curve/discrete.h"
#include "renderer/stages/render_entity.h"
#include "util/vector.h"


namespace openage::renderer::terrain {

/**
 * Render entity for pushing updates to the Terrain renderer.
 */
class RenderEntity final : public renderer::RenderEntity {
public:
	RenderEntity();
	~RenderEntity() = default;

	using terrain_elevation_t = util::FixedPoint<uint64_t, 16>;
	using tiles_t = std::vector<std::pair<terrain_elevation_t, std::string>>;

	/**
	 * Update a single tile of the displayed terrain (chunk) with information from the
	 * gamestate.
	 *
	 * Updating the render entity with this method is thread-safe.
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
	 * Updating the render entity with this method is thread-safe.
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
	 * Accessing the terrain vertices is thread-safe.
	 *
	 * @return Vector of vertex coordinates.
	 */
	const std::vector<coord::scene3> get_vertices();

	/**
	 * Get the tiles of the terrain.
	 *
	 * Accessing the terrain tiles is thread-safe.
	 *
	 * @return Terrain tiles.
	 */
	const tiles_t get_tiles();

	/**
	 * Get the terrain paths used in the terrain.
	 *
	 * Accessing the terrain paths is thread-safe.
	 *
	 * @return Terrain paths.
	 */
	const std::unordered_set<std::string> get_terrain_paths();

	/**
	 * Get the number of vertices on each side of the terrain.
	 *
	 * Accessing the vertices size is thread-safe.
	 *
	 * @return Vector with width as first element and height as second element.
	 */
	const util::Vector2s get_size();

private:
	/**
	 * Chunk dimensions (width x height).
	 */
	util::Vector2s size;

	/**
	 * Terrain tile information (elevation, terrain path).
	 */
	tiles_t tiles;

	/**
	 * Terrain texture paths used in \p tiles .
	 */
	std::unordered_set<std::string> terrain_paths;

	/**
	 * Terrain vertices (ingame coordinates).
	 */
	std::vector<coord::scene3> vertices;
};
} // namespace openage::renderer::terrain
