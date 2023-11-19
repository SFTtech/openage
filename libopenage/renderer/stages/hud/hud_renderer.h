// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <shared_mutex>
#include <vector>

#include "util/path.h"

namespace openage {

namespace time {
class Clock;
}

namespace renderer {
class Renderer;
class RenderPass;
class ShaderProgram;
class Texture2d;
class Window;

namespace camera {
class Camera;
}

namespace resources {
class AssetManager;
}

namespace hud {
class HudObject;
class HudRenderEntity;

/**
 * Renderer for the "Heads-Up Display" (HUD).
 * Draws UI elements that are not part of the GUI, e.g. health bars, selection boxes, minimap, etc.
 */
class HudRenderer {
public:
	HudRenderer(const std::shared_ptr<Window> &window,
	            const std::shared_ptr<renderer::Renderer> &renderer,
	            const std::shared_ptr<renderer::camera::Camera> &camera,
	            const util::Path &shaderdir,
	            const std::shared_ptr<renderer::resources::AssetManager> &asset_manager,
	            const std::shared_ptr<time::Clock> clock);
	~HudRenderer() = default;

	/**
	 * Get the render pass of the HUD renderer.
	 *
	 * @return Render pass for HUD drawing.
	 */
	std::shared_ptr<renderer::RenderPass> get_render_pass();

	/**
	 * Add a new render entity of the HUD renderer.
	 *
	 * @param render_entity New render entity.
	 */
	void add_render_entity(const std::shared_ptr<HudRenderEntity> entity);

	/**
	 * Update the render entities and render positions.
	 */
	void update();

	/**
	 * Resize the FBO for the HUD rendering. This basically updates the output
     * texture size.
	 *
	 * @param width New width of the FBO.
	 * @param height New height of the FBO.
	 */
	void resize(size_t width, size_t height);

private:
	/**
	 * Create the render pass for HUD drawing.
	 *
	 * Called during initialization of the HUD renderer.
	 *
	 * @param width Width of the FBO.
	 * @param height Height of the FBO.
	 * @param shaderdir Directory containg the shader source files.
	 */
	void initialize_render_pass(size_t width,
	                            size_t height,
	                            const util::Path &shaderdir);

	/**
	 * Reference to the openage renderer.
	 */
	std::shared_ptr<renderer::Renderer> renderer;

	/**
	 * Camera for model uniforms.
	 */
	std::shared_ptr<renderer::camera::Camera> camera;

	/**
	 * Texture manager for loading assets.
	 */
	std::shared_ptr<renderer::resources::AssetManager> asset_manager;

	/**
	 * Render pass for the HUD drawing.
	 */
	std::shared_ptr<renderer::RenderPass> render_pass;

	/**
	 * Render entities requested by the game simulation or input system.
	 */
	std::vector<std::shared_ptr<HudObject>> render_objects;

	/**
	 * Shader for rendering the HUD objects.
	 */
	std::shared_ptr<renderer::ShaderProgram> display_shader;

	/**
	 * Simulation clock for timing animations.
	 */
	std::shared_ptr<time::Clock> clock;

	/**
	 * Output texture.
	 */
	std::shared_ptr<renderer::Texture2d> output_texture;

	/**
	 * Depth texture.
	 */
	std::shared_ptr<renderer::Texture2d> depth_texture;

	/**
	 * Mutex for protecting threaded access.
	 */
	std::shared_mutex mutex;
};
} // namespace hud
} // namespace renderer
} // namespace openage
