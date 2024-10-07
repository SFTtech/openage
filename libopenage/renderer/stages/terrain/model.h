// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "coord/scene.h"
#include "time/time.h"
#include "util/vector.h"


namespace openage::renderer {

namespace camera {
class Camera;
}

namespace resources {
class AssetManager;
}

namespace terrain {
class RenderEntity;
class TerrainRenderMesh;
class TerrainChunk;

/**
 * 3D model of the whole terrain. Combines the individual meshes
 * into one single structure.
 */
class TerrainRenderModel {
public:
	/**
	 * Create a new model for the terrain.
	 *
	 * @param asset_manager Asset manager for loading resources.
	 */
	TerrainRenderModel(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager);
	~TerrainRenderModel() = default;

	/**
	 * Add a new chunk to the terrain model.
	 *
	 * @param entity Render entity of the chunk.
	 * @param chunk_size Size of the chunk in tiles.
	 * @param chunk_offset Offset of the chunk from origin in tiles.
	 */
	void add_chunk(const std::shared_ptr<RenderEntity> &entity,
	               const util::Vector2s chunk_size,
	               const coord::scene2_delta chunk_offset);

	/**
	 * Set the current camera of the scene.
	 *
	 * @param camera Camera object viewing the scene.
	 */
	void set_camera(const std::shared_ptr<renderer::camera::Camera> &camera);

	/**
	 * Fetch updates from the render entity.
	 *
	 * @param time Current simulation time.
	 */
	void fetch_updates(const time::time_t &time = 0.0);

	/**
	 * Update the uniforms of the renderable associated with this object.
	 *
	 * @param time Current simulation time.
	 */
	void update_uniforms(const time::time_t &time = 0.0);

	/**
	 * Get the chunks composing the terrain.
	 *
	 * @return Chunks of the terrain.
	 */
	const std::vector<std::shared_ptr<TerrainChunk>> &get_chunks() const;

private:
	/**
	 * Chunks composing the terrain.
	 */
	std::vector<std::shared_ptr<TerrainChunk>> chunks;

	/**
	 * Camera for view and projection uniforms.
	 */
	std::shared_ptr<renderer::camera::Camera> camera;

	/**
	 * Asset manager for central accessing and loading textures.
	 */
	std::shared_ptr<renderer::resources::AssetManager> asset_manager;
};

} // namespace terrain
} // namespace openage::renderer
