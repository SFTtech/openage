// Copyright 2018-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <vector>

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
	 * Total size of the map
	 * origin is the left corner
	 * x = top left edge; y = top right edge
	 */
	util::Vector2s size;

	/**
	 * Subdivision of the main terrain entity.
	 */
	std::vector<std::shared_ptr<TerrainChunk>> chunks;
};

} // namespace gamestate
} // namespace openage
