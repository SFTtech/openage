// Copyright 2022-2023 the openage authors. See copying.md for legal info.

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

namespace terrain {
class TerrainRenderEntity;

/**
 * Drawable chunk of terrain with a single texture.
 */
class TerrainRenderMesh {
public:
	/**
     * Create a new terrain render mesh with empty values.
     *
     * Mesh and texture ave to be set before the terrain mesh becomes renderable.
     */
	TerrainRenderMesh();

	/**
     * Create a new terrain render mesh.
     *
     * @param texture Texture drawn onto the mesh.
     * @param mesh Vertex data of the mesh.
     */
	TerrainRenderMesh(const std::shared_ptr<renderer::Texture2d> &texture,
	                  renderer::resources::MeshData &&mesh);

	~TerrainRenderMesh() = default;

	/**
     * Set the reference to the uniform inputs of the renderable
     * associated with this mesh. Relevant uniforms are updated
     * when calling \p update().
     *
     * @param uniforms Uniform inputs of this mesh's renderable.
     */
	void set_uniforms(const std::shared_ptr<renderer::UniformInput> &uniforms);

	/**
     * Get the reference to the uniform inputs of the mesh's renderable.
     *
     * @return Uniform inputs of the renderable.
     */
	const std::shared_ptr<renderer::UniformInput> &get_uniforms();

	/**
     * Set the vertex mesh for the terrain.
     *
     * @param mesh Mesh for creating a renderer geometry object.
     */
	void set_mesh(renderer::resources::MeshData &&mesh);

	/**
     * Get the vertex mesh for the terrain.
     *
     * @return Mesh for creating a renderer geometry object.
     */
	const renderer::resources::MeshData &get_mesh();

	/**
     * Set the texture that should be drawn onto the mesh.
     *
     * @param texture Texture object.
     */
	void set_texture(const std::shared_ptr<renderer::Texture2d> &texture);

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
	 * Get the model transformation matrix for rendering.
	 *
	 * @return Model matrix.
	 */
	Eigen::Matrix4f get_model_matrix();

	/**
	 * Check whether the mesh or texture were changed.
	 *
	 * @return true if changes were made, else false.
	 */
	bool is_changed();

	/**
	 * Clear the update flag by setting it to false.
	 */
	void clear_changed_flag();

private:
	/**
     * Stores whether a new renderable for this mesh needs to be created
     * for the render pass.
     */
	bool require_renderable;

	/**
	 * Stores whether the the mesh or the texture are updated.
	 */
	bool changed;

	/**
     * Texture used for the mesh.
     */
	std::shared_ptr<renderer::Texture2d> texture;

	/**
     * Shader uniforms for the renderable in the terrain render pass.
     */
	std::shared_ptr<renderer::UniformInput> uniforms;

	/**
     * Pre-transformation vertices for the terrain model.
     */
	renderer::resources::MeshData mesh;
};
} // namespace terrain
} // namespace openage::renderer
