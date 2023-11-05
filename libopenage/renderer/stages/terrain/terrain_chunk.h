// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "time/time.h"
#include "util/vector.h"


namespace openage::renderer {

namespace resources {
class AssetManager;
}

namespace terrain {
class TerrainRenderMesh;
class TerrainRenderEntity;

const size_t MAX_CHUNK_WIDTH = 16;
const size_t MAX_CHUNK_HEIGHT = 16;


class TerrainChunk {
public:
	/**
     * Create a new terrain chunk.
     *
     * @param asset_manager Asset manager for loading textures.
     */
	TerrainChunk(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager,
	             const util::Vector2s size,
	             const util::Vector2s offset);

	~TerrainChunk() = default;

	/**
     * Set the terrain render entity for vertex updates of this mesh.
     *
     * @param entity New terrain render entity.
     */
	void set_render_entity(const std::shared_ptr<TerrainRenderEntity> &entity);

	/**
     * Fetch updates from the render entity.
     *
     * @param time Current simulation time.
     */
	void fetch_updates(const time::time_t &time = 0.0);

	/**
     * Update the uniforms of the meshes.
     *
     * @param time Current simulation time.
     */
	void update_uniforms(const time::time_t &time = 0.0);

	/**
     * Get the meshes composing the terrain.
     *
     * @return Vector of terrain meshes.
     */
	const std::vector<std::shared_ptr<TerrainRenderMesh>> &get_meshes() const;

private:
	/**
	 * Create a terrain mesh from the data provided by the render entity.
	 *
	 * @return New terrain mesh.
	 */
	std::shared_ptr<TerrainRenderMesh> create_mesh();

	/**
     * Size of the chunk in tiles (width x height).
     */
	util::Vector2s size;

	/**
     * Offset of the chunk from origin in tiles (x, y).
     */
	util::Vector2s offset;

	/**
	 * Meshes composing the terrain. Each mesh represents a drawable vertex surface
	 * and a texture.
	 */
	std::vector<std::shared_ptr<TerrainRenderMesh>> meshes;

	/**
	 * Asset manager for central accessing and loading textures.
     */
	std::shared_ptr<renderer::resources::AssetManager> asset_manager;

	/**
     * Source for ingame terrain coordinates. These coordinates are translated into
     * our render vertex mesh when \p update() is called.
     */
	std::shared_ptr<TerrainRenderEntity> render_entity;
};


} // namespace terrain
} // namespace openage::renderer
