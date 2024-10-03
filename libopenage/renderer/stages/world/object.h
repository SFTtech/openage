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
class Frustum2d;
}

namespace resources {
class AssetManager;
class Animation2dInfo;
} // namespace resources

namespace world {
class RenderEntity;

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
	void set_render_entity(const std::shared_ptr<RenderEntity> &entity);

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
	 * Since the object is a bunch of sprite layers, the mesh is always a quad.
	 *
	 * @return Mesh for creating a renderer geometry object.
	 */
	static const renderer::resources::MeshData get_mesh();

	/**
	 * Get the model matrix for the uniform input of a layer.
	 *
	 * @return Model matrix.
	 */
	static const Eigen::Matrix4f get_model_matrix();

	/**
	 * Check whether a new renderable needs to be created for this mesh.
	 *
	 * If true, the old renderable should be removed from the render pass.
	 * The updated uniforms and geometry should be passed to this mesh.
	 * Afterwards, clear the requirement flag with \p clear_requires_renderable().
	 *
	 * @return true if a new renderable is required, else false.
	 */
	bool requires_renderable() const;

	/**
	 * Indicate to this mesh that a new renderable has been created.
	 */
	void clear_requires_renderable();

	/**
	 * Get the number of layers required by this object.
	 *
	 * @return Number of layers.
	 */
	size_t get_required_layer_count(const time::time_t &time) const;

	std::vector<size_t> get_layer_positions(const time::time_t &time) const;

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
	 * Set the uniform inputs for the layers of this object.
	 * Layer uniforms are updated on every update call.
	 *
	 * @param uniforms Uniform inputs of this object's layers.
	 */
	void set_uniforms(std::vector<std::shared_ptr<renderer::UniformInput>> &&uniforms);

	/**
	 * Check whether the object is visible in the camera view.
	 *
	 * @param frustum Camera frustum for culling.
	 * @param time Current simulation time.
	 *
	 * @return true if the object is visible, else false.
	 */
	bool is_visible(const camera::Frustum2d &frustum,
	                const time::time_t &time);

	/**
	 * Shader uniform IDs for setting uniform values.
	 */
	inline static uniform_id_t obj_world_position;
	inline static uniform_id_t flip_x;
	inline static uniform_id_t flip_y;
	inline static uniform_id_t tex;
	inline static uniform_id_t tile_params;
	inline static uniform_id_t scale;
	inline static uniform_id_t subtex_size;
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
	 * Asset manager for central accessing and loading asset resources.
	 */
	std::shared_ptr<renderer::resources::AssetManager> asset_manager;

	/**
	 * Entity that gets updates from the gamestate, e.g. the position and
	 * requested animation data.
	 */
	std::shared_ptr<RenderEntity> render_entity;

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
	 * Animation information for the layers.
	 */
	curve::Discrete<std::shared_ptr<renderer::resources::Animation2dInfo>> animation_info;

	/**
	 * Shader uniforms for the layers of the object. Each layer corresponds to a
	 * renderable in the render pass.
	 */
	std::vector<std::shared_ptr<renderer::UniformInput>> layer_uniforms;

	/**
	 * Time of the last update call.
	 */
	time::time_t last_update;
};
} // namespace world
} // namespace openage::renderer
