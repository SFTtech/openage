// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <eigen3/Eigen/Dense>
#include <memory>

namespace openage::renderer {
class Renderer;

namespace camera {
class Camera;
}

namespace resources {
class TextureManager;
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
	TerrainRenderModel(const std::shared_ptr<renderer::Renderer> &renderer);
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

	void update();

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
	 * Texture manager for central accessing and loading textures.
	 *
     * TODO: Replace with asset manager
     */
	std::shared_ptr<renderer::resources::TextureManager> texture_manager;

	/**
     * Source for ingame terrain coordinates. These coordinates are translated into
     * our render vertex mesh when \p update() is called.
     */
	std::shared_ptr<TerrainRenderEntity> render_entity;
};

} // namespace terrain
} // namespace openage::renderer