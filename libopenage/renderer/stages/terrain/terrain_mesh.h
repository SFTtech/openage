// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "renderer/resources/mesh_data.h"
#include <eigen3/Eigen/Dense>

namespace openage::renderer {
class Geometry;
class Renderer;
class Texture2d;
class UniformInput;

namespace resources {
class TextureManager;
}

namespace terrain {
class TerrainRenderEntity;

/**
 * Drawable chunk of terrain with a single texture.
 */
class TerrainRenderMesh {
public:
	/**
     * Create a new terrain render mesh.
     *
     * @param entity Terrain render entity for updating vertex coordinates.
     */
	TerrainRenderMesh(const std::shared_ptr<renderer::Renderer> &renderer,
	                  const std::shared_ptr<TerrainRenderEntity> &entity);
	~TerrainRenderMesh() = default;

	/**
     * Set the terrain render entity for vertex updates of this mesh.
     *
     * @param entity New terrain render entity.
     */
	void set_render_entity(const std::shared_ptr<TerrainRenderEntity> &entity);

	/**
     * Recalculate the vertex positions for this mesh with information
     * from the currently set terrain render entity.
     */
	void update();

	/**
     * Get the vertex mesh for the terrain.
     *
     * @return Mesh for creating a renderer geometry object.
     */
	const renderer::resources::MeshData &get_mesh();

	/**
      * Get the texture that should be drawn onto the mesh.
      *
      * @return Texture object.
      */
	const std::shared_ptr<renderer::Texture2d> &get_texture();

	/**
      * Check whether a new renderable needs to be created for this mesh.
      * 
      * If true, the old renderable should be removed from the render pass.
      * The updated uniforms and geometry should be passed to this mesh.
      * Afterwards, clear the requirement flag with \p clear_requires_renderable().
      * 
      * @return true if a new renderable is required, else false.
      */
	bool requires_renderable();

	/**
      * Indicate to this mesh that a new renderable has been created.
      */
	void clear_requires_renderable();

	/**
	 * Check whether the mesh or texture were changed by \p update().
	 * 
	 * @return true if changes were made, else false.
	 */
	bool is_changed();

	/**
	 * Clear the update flag by setting it to false.
	 */
	void clear_changed_flag();

	/**
      * Set the reference to the uniform inputs of the renderable
      * associated with this mesh. Relevant uniforms are updated
      * when calling \p update().
      * 
      * @param uniforms Uniform inputs of this mesh's renderable.
      */
	void set_uniforms(const std::shared_ptr<renderer::UniformInput> &uniforms);

private:
	/**
      * Stores whether a new renderable for this mesh needs to be created
      * for the render pass.
      */
	bool require_renderable;

	/**
	 * Stores whether the \p update() call changed the mesh or the texture
      * value.
	 */
	bool changed;

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

	/**
      * Texture used for the mesh.
      */
	std::shared_ptr<renderer::Texture2d> texture;

	/**
     * Pre-transformation vertices for the terrain model.
     */
	renderer::resources::MeshData mesh;

	/**
      * Shader uniforms for the renderable in the terrain render pass.
      */
	std::shared_ptr<renderer::UniformInput> uniforms;
};
} // namespace terrain
} // namespace openage::renderer