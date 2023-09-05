// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "time/time.h"


namespace openage::renderer {

namespace camera {
class Camera;
}

namespace resources {
class AssetManager;
}

namespace terrain {
class TerrainRenderEntity;
class TerrainRenderMesh;

/**
 * 3D model of the whole terrain. Combines the individual meshes
 * into one single structure.
 */
class TerrainRenderModel {
public:
	TerrainRenderModel(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager);
	~TerrainRenderModel() = default;

	/**
     * Set the terrain render entity for vertex updates of this mesh.
     *
     * @param entity New terrain render entity.
     */
	void set_render_entity(const std::shared_ptr<TerrainRenderEntity> &entity);

	/**
     * Set the current camera of the scene.
     *
     * @param camera Camera object viewing the scene.
     */
	void set_camera(const std::shared_ptr<renderer::camera::Camera> &camera);

	/**
     * Fetch updates from the render entity.
     */
	void fetch_updates();

	/**
     * Update the uniforms of the renderable associated with this object.
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
	 * Meshes composing the terrain. Each mesh represents a drawable vertex surface
	 * and a texture.
	 */
	std::vector<std::shared_ptr<TerrainRenderMesh>> meshes;

	/**
	 * Camera for view and projection uniforms.
	 */
	std::shared_ptr<renderer::camera::Camera> camera;

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
