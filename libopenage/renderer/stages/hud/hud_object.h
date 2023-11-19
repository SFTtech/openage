// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <string>

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

namespace hud {
class HudRenderEntity;

class HudObject {
public:
	HudObject(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager);
	~HudObject() = default;

	/**
     * Set the world render entity.
     *
     * @param entity New world render entity.
     */
	void set_render_entity(const std::shared_ptr<HudRenderEntity> &entity);

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
	std::shared_ptr<HudRenderEntity> render_entity;

	/**
     * Shader uniforms for the renderable in the terrain render pass.
     */
	std::shared_ptr<renderer::UniformInput> uniforms;

	/**
	 * Time of the last update call.
	 */
	time::time_t last_update;
};
} // namespace hud
} // namespace openage::renderer
