// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "util/path.h"
#include "util/vector.h"

namespace openage::renderer {
// ASDF: testing
class Renderer;
class Texture2d;

namespace terrain {

struct TerrainVertex {
	float x;
	float y;
	float height;
};

class TerrainRenderEntity {
public:
	TerrainRenderEntity(const std::shared_ptr<renderer::Renderer> &renderer,
	                    const util::Path &assetdir);
	~TerrainRenderEntity() = default;

	/**
     * TODO: Update the render entity with information from the
     * gamestate.
     */
	void update();

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
	const std::shared_ptr<renderer::Texture2d> &get_textures();

	/**
     * Get the number of vertices on each side of the terrain.
     *
     * @return Vector with width as first element and height as second element.
     */
	const util::Vector2s &get_size();

private:
	/**
     * Terrain dimensions (width x height).
     */
	util::Vector2s size;

	std::vector<TerrainVertex> vertices;
	std::shared_ptr<renderer::Texture2d> texture;
	// std::unordered_map<Texture2d, size_t> texture_map; // texture -> vertex indices

	// ASDF: testing
	std::shared_ptr<renderer::Renderer> renderer;
	util::Path assetdir;
};
} // namespace terrain
} // namespace openage::renderer