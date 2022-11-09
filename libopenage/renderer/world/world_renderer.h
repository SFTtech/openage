// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "util/path.h"

namespace openage::renderer {
class Renderer;
class RenderPass;
class Window;

namespace world {

/**
 * @brief Renderer for drawing and displaying entities in the game world (units, buildings, etc.)
 * 
 */
class WorldRenderer {
public:
	WorldRenderer(const std::shared_ptr<Window> &window,
	              const std::shared_ptr<renderer::Renderer> &renderer,
	              const util::Path &shaderdir);
	~WorldRenderer() = default;

	/**
	 * Get the render pass of the world renderer.
	 *
	 * @return Render pass for world drawing.
	 */
	std::shared_ptr<renderer::RenderPass> get_render_pass();

	/**
	 * Update the render entities and render positions.
	 */
	void update();

	/**
	 * Resize the FBO for the world rendering. This basically updates the output
     * texture size.
	 *
	 * @param width New width of the FBO.
	 * @param height New height of the FBO.
	 */
	void resize(size_t width, size_t height);

private:
	/**
	 * Create the render pass for world drawing.
	 *
	 * Called during initialization of the world renderer.
	 *
	 * @param shaderdir Directory containg the shader source files.
	 */
	void initialize_render_pass(const util::Path &shaderdir);

	// Render entities requested by the game world
	// std::vector<std::shared_ptr<WorldRenderEntity>> render_entities;

	/**
	 * Render pass for the terrain drawing.
	 */
	std::shared_ptr<renderer::RenderPass> render_pass;

	/**
	 * Reference to the openage renderer.
	 */
	std::shared_ptr<renderer::Renderer> renderer;
};
} // namespace world
} // namespace openage::renderer