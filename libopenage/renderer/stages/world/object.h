// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <string>

#include "coord/scene.h"
#include "curve/continuous.h"
#include "curve/discrete.h"
#include "curve/segmented.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/types.h"
#include "time/time.h"


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

/**
 * Stores the state of a renderable object in the World render stage.
 */
class WorldObject {
public:
	/**
	 * Create a new object for the World render stage.
	 *
	 * @param asset_manager Asset manager for loading resources.
	 */
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
	 * Get the ID of the corresponding game entity.
	 *
	 * @return Game entity ID.
	 */
	uint32_t get_id();

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

	bool within_camera_frustum(const std::shared_ptr<camera::Camera> &camera);

	/**
	 * Shader uniform IDs for setting uniform values.
	 */
	inline static uniform_id_t obj_world_position;
	inline static uniform_id_t flip_x;
	inline static uniform_id_t flip_y;
	inline static uniform_id_t tex;
	inline static uniform_id_t tile_params;
	inline static uniform_id_t scale;
	inline static uniform_id_t anchor_offset;

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

	/**
	 * Time of the last update call.
	 */
	time::time_t last_update;
};
} // namespace world
} // namespace openage::renderer
