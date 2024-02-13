// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "util/path.h"

namespace openage::renderer {
class Renderer;
class RenderPass;
class RenderTarget;
class ShaderProgram;
class Texture2d;
class Window;

namespace screen {

/**
 * Draws the results of previous render passes to the window screen
 * (i.e. the renderers display target). This should always be the
 * last render stage.
 */
class ScreenRenderStage {
public:
	/**
	 * Create a new render stage for drawing to the screen.
	 *
	 * @param window openage window targeted for rendering.
	 * @param renderer openage low-level renderer.
	 * @param shaderdir Directory containing the shader source files.
	 */
	ScreenRenderStage(const std::shared_ptr<Window> &window,
	                  const std::shared_ptr<renderer::Renderer> &renderer,
	                  const util::Path &shaderdir);
	~ScreenRenderStage() = default;

	/**
	 * Get the render pass of the screen renderer.
	 *
	 * @return Render pass for screen drawing.
	 */
	std::shared_ptr<renderer::RenderPass> get_render_pass();

	/**
	 * Set the render targets whose outputs should be drawn into the
	 * display target.
	 *
	 * Targets are drawn in the order they appear in the vector.
	 * Alpha blending is enabled.
	 *
	 * TODO: Replace this with a \p set_textures() method that
	 * only receives the output textures.
	 *
	 * @param targets Render targets that should be drawn.
	 */
	void set_render_targets(const std::vector<std::shared_ptr<renderer::RenderTarget>> &targets);

private:
	/**
	 * Create the render pass for the screen output.
	 *
	 * Called during initialization of the screen renderer.
	 *
	 * @param shaderdir Directory containg the shader source files.
	 */
	void initialize_render_pass(const util::Path &shaderdir);

	/**
	 * Recreates the renderables and the render pass of the screen renderer
	 * when \p set_render_targets() is called. This is necessary to take
	 * the textures of the new targets into account.
	 */
	void update_render_pass();

	/**
	 * Reference to the openage renderer.
	 */
	std::shared_ptr<renderer::Renderer> renderer;

	/**
	 * Render pass for the screen drawing.
	 */
	std::shared_ptr<renderer::RenderPass> render_pass;

	/**
	 * Output texture.
	 */
	std::shared_ptr<renderer::Texture2d> output_texture;

	/**
	 * FBO render targets that are drawn to the screen.
	 *
	 * TODO: Use pass_outputs instead
	 */
	std::vector<std::shared_ptr<renderer::RenderTarget>> render_targets;

	/**
	 * Texture targets of the individual FBOs.
	 */
	std::vector<std::shared_ptr<renderer::Texture2d>> pass_outputs;

	/**
	 * Shader for rendering to the window.
	 */
	std::shared_ptr<renderer::ShaderProgram> display_shader;
};

} // namespace screen
} // namespace openage::renderer
