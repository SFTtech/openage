// Copyright 2018-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "coord/chunk.h"
#include "util/vector.h"


namespace openage {
namespace renderer {
class RenderFactory;
} // namespace renderer

namespace gamestate {
class TerrainChunk;

/**
 * Entity for managing the map terrain of a game.
 */
class Terrain {
public:
	/**
	 * Create a new terrain.
	 */
	Terrain();
	~Terrain() = default;

	/**
	 * Create a new terrain.
	 *
	 * Chunks must conform to these constraints:
	 *  - All chunks that are not last in a row OR columns must have the same size (width x height).
	 *  - All chunks that are not last in a row OR columns must be square (width == height).
	 *  - The last chunk in a row may have a different width.
	 *  - The last chunk in a column may have a different height.
	 *
	 * @param size Total size of the terrain in tiles (width x height).
	 * @param chunks Terrain chunks.
	 */
	Terrain(const util::Vector2s &size,
	        std::vector<std::shared_ptr<TerrainChunk>> &&chunks);

	/**
	 * Get the size of the terrain (in tiles).
	 *
	 * @return Terrain tile size (width x height).
	 */
	const util::Vector2s &get_size() const;

	/**
	 * Get the size of the terrain (in chunks).
	 *
	 * @return Terrain chunk size (width x height).
	 */
	const util::Vector2s get_chunks_size() const;

	/**
	 * Add a chunk to the terrain.
	 *
	 * @param chunk New chunk.
	 */
	void add_chunk(const std::shared_ptr<TerrainChunk> &chunk);

	/**
	 * Get the chunks of the terrain.
	 *
	 * @return Terrain chunks.
	 */
	const std::vector<std::shared_ptr<TerrainChunk>> &get_chunks() const;

	/**
	 * Get a specific chunk of the terrain.
	 *
	 * @param idx Index of the chunk.
	 *
	 * @return Terrain chunk.
	 */
	const std::shared_ptr<TerrainChunk> &get_chunk(size_t idx) const;

	/**
	 * Get a specific chunk of the terrain.
	 *
	 * @param pos Position of the chunk in the terrain.
	 *
	 * @return Terrain chunk.
	 */
	const std::shared_ptr<TerrainChunk> &get_chunk(const coord::chunk &pos) const;

	/**
	 * Attach a renderer which enables graphical display.
	 *
	 * TODO: We currently have to do attach this here too in addition to the terrain
	 *       factory because the renderer gets attached AFTER the terrain is
	 *       already created. In the future, the game should wait for the renderer
	 *       before creating the terrain.
	 *
	 * @param render_factory Factory for creating connector objects for gamestate->renderer
	 *                       communication.
	 */
	void attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory);

private:
	/**
	 * Total size of the terrain in tiles (width x height).
	 *
	 * Origin is the top left corner (left corner with camera projection).
	 */
	util::Vector2s size;

	/**
	 * Subdivision of the main terrain entity.
	 *
	 * Ordered in rows from left to right, top to bottom.
	 */
	std::vector<std::shared_ptr<TerrainChunk>> chunks;
};

} // namespace gamestate
} // namespace openage
