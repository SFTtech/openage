// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "util/vector.h"

namespace openage {
namespace renderer::terrain {
class TerrainRenderEntity;
}

namespace gamestate {
class TerrainChunk;

/**
 * Entity for managing the map terrain of a game.
 */
class Terrain {
public:
	Terrain(const std::string &texture_path);
	~Terrain() = default;

	/**
	 * Set the current render entity of the terrain.
	 *
	 * @param entity New render entity.
	 */
	void set_render_entity(const std::shared_ptr<renderer::terrain::TerrainRenderEntity> &entity);

	const std::vector<std::shared_ptr<TerrainChunk>> &get_chunks() const;

	// TODO: This should be an event
	void generate();

private:
	// test connection to renderer
	void render_update();

	/**
     *  Total size of the map
	 * origin is the left corner
	 * x = top left edge; y = top right edge
     */
	util::Vector2s size;

	/**
     * Subdivision of the main terrain entity.
     */
	std::vector<std::shared_ptr<TerrainChunk>> chunks;

	// ASDF: Move these members into terrain chunk
	// Heights of the terrain grid
	std::vector<float> height_map;
	// path to a texture
	std::string texture_path;
	// render entity for pushing updates to
	std::shared_ptr<renderer::terrain::TerrainRenderEntity> render_entity;
};

} // namespace gamestate
} // namespace openage
