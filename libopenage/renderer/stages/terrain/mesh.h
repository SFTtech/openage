// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>

#include <eigen3/Eigen/Dense>

#include "coord/scene.h"
#include "renderer/resources/mesh_data.h"
#include "time/time.h"
#include "util/vector.h"


namespace openage::renderer {
class UniformInput;

namespace resources {
class AssetManager;
class TerrainInfo;
} // namespace resources

namespace terrain {

/**
 * Drawable chunk of terrain with a single texture.
 */
class TerrainRenderMesh {
public:
	/**
	 * Create a new terrain render mesh with empty values.
	 *
	 * Mesh and texture need to be set before the terrain mesh becomes renderable.
	 *
	 * @param asset_manager Asset manager for central accessing and loading textures.
	 */
	TerrainRenderMesh(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager);

	/**
	 * Create a new terrain render mesh.
	 *
	 * @param asset_manager Asset manager for central accessing and loading textures.
	 * @param info Terrain info for the renderable.
	 * @param mesh Vertex data of the mesh.
	 */
	TerrainRenderMesh(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager,
	                  const std::shared_ptr<renderer::resources::TerrainInfo> &info,
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
	 * Set the terrain info that is drawn onto the mesh.
	 *
	 * @param texture Terrain info.
	 */
	void set_terrain_info(const std::shared_ptr<renderer::resources::TerrainInfo> &info);

	/**
	 * Update the uniforms of the renderable associated with this object.
	 *
	 * @param time Current simulation time.
	 */
	void update_uniforms(const time::time_t &time = 0.0);

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
	 * Create the model transformation matrix for rendering.
	 *
	 * @param offset Offset of the terrain mesh to the scene origin.
	 */
	void create_model_matrix(const coord::scene2_delta &offset);

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
	 * Asset manager for central accessing and loading textures.
	 */
	std::shared_ptr<renderer::resources::AssetManager> asset_manager;

	/**
	 * Terrain information for the renderables.
	 */
	std::shared_ptr<renderer::resources::TerrainInfo> terrain_info;

	/**
	 * Shader uniforms for the renderable in the terrain render pass.
	 */
	std::shared_ptr<renderer::UniformInput> uniforms;

	/**
	 * Pre-transformation vertices for the terrain model.
	 */
	renderer::resources::MeshData mesh;

	/**
	 * Transformation matrix for the terrain model.
	 */
	Eigen::Matrix4f model_matrix;
};
} // namespace terrain
} // namespace openage::renderer
