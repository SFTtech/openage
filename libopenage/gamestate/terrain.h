// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "util/path.h"

namespace openage {
namespace renderer::terrain {
class TerrainRenderEntity;
}

namespace gamestate {

/**
 * Position inside the map coordinates.
 */
struct TerrainNode {
	float x;
	float y;
	float h;
};

/**
 * Entity for managing the map terrain of a game.
 */
class Terrain {
public:
	Terrain();
	~Terrain() = default;

private:
	// test connection to renderer
	void push_to_render();

	// ASDF: testing
	// Vertexes of the terrain grid
	std::vector<std::vector<TerrainNode>> height_map;
	// path to a texture
	util::Path texture_path;

	// render entity for pushing updates to
	std::shared_ptr<renderer::terrain::TerrainRenderEntity> render_entity;
};

} // namespace gamestate
} // namespace openage