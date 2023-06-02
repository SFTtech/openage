// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <eigen3/Eigen/Dense>

#include "coord/scene.h"
#include "curve/continuous.h"
#include "curve/curve.h"
#include "curve/discrete.h"
#include "curve/segmented.h"
#include "renderer/resources/mesh_data.h"

namespace openage::renderer {
class UniformInput;

namespace camera {
class Camera;
}

namespace resources {
class AssetManager;
class Animation2dInfo;
} // namespace resources

namespace world {
class WorldRenderEntity;

class WorldObject {
public:
	WorldObject(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager);
	~WorldObject() = default;

	/**
     * Set the world render entity.
     *
     * @param entity New world render entity.
     */
	void set_render_entity(const std::shared_ptr<WorldRenderEntity> &entity);

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
	void update_uniforms(const curve::time_t &time = 0.0);

	/**
	 * Get the ID of the corresponding game entity.
	 *
	 * @return Game entity ID.
	 */
	uint32_t get_id();

	/**
	 * Get the position of the object inside the scene.
	 *
	 * @return Position of the object.
	 */
	const curve::Continuous<coord::scene3> get_position();

	/**
     * Get the quad for creating the geometry.
     *
     * @return Mesh for creating a renderer geometry object.
     */
	static const renderer::resources::MeshData get_mesh();

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
	 * Check whether the object was changed by \p update().
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
     * associated with this object. Relevant uniforms are updated
     * when calling \p update().
     *
     * @param uniforms Uniform inputs of this object's renderable.
     */
	void set_uniforms(const std::shared_ptr<renderer::UniformInput> &uniforms);

private:
	/**
     * Stores whether a new renderable for this object needs to be created
     * for the render pass.
     */
	bool require_renderable;

	/**
	 * Stores whether the \p update() call changed the object.
	 */
	bool changed;

	/**
	 * Camera for model uniforms.
	 */
	std::shared_ptr<renderer::camera::Camera> camera;

	/**
	 * Asset manager for central accessing and loading asset resources.
	 */
	std::shared_ptr<renderer::resources::AssetManager> asset_manager;

	/**
	 * Source for positional and texture data.
	 */
	std::shared_ptr<WorldRenderEntity> render_entity;

	/**
	 * Reference ID for passing interaction with the graphic (e.g. mouse clicks) back to
	 * the engine.
	 */
	uint32_t ref_id;

	/**
	 * Position of the object.
	 */
	curve::Continuous<coord::scene3> position;

	/**
     * Angle of the object.
     */
	curve::Segmented<coord::phys_angle_t> angle;

	/**
     * Animation information for the renderables.
     */
	curve::Discrete<std::shared_ptr<renderer::resources::Animation2dInfo>> animation_info;

	/**
     * Shader uniforms for the renderable in the terrain render pass.
     */
	std::shared_ptr<renderer::UniformInput> uniforms;
};
} // namespace world
} // namespace openage::renderer
