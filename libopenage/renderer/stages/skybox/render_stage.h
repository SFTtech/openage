// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include <eigen3/Eigen/Dense>

#include "util/path.h"

namespace openage::renderer {
class Renderer;
class RenderPass;
class Texture2d;
class UniformInput;
class Window;

namespace skybox {

/**
 * Draws the dark background thingy behind the terrain.
 *
 * (Technically not a skybox because we look down from above into
 * a black devilish void of nothingness. Maybe "hellbox" is more
 * appropriate.)
 */
class SkyboxRenderStage {
public:
	/**
	 * Create a new render stage for the skybox.
	 *
	 * @param window openage window targeted for rendering.
	 * @param renderer openage low-level renderer.
	 * @param shaderdir Directory containing the shader source files.
	 */
	SkyboxRenderStage(const std::shared_ptr<Window> &window,
	                  const std::shared_ptr<renderer::Renderer> &renderer,
	                  const util::Path &shaderdir);
	~SkyboxRenderStage() = default;

	/**
	 * Get the render pass of the skybox renderer.
	 *
	 * @return Render pass for skybox drawing.
	 */
	std::shared_ptr<renderer::RenderPass> get_render_pass();

	/**
	 * Set the color used for the skybox using integer values
	 * for each channel (0 to 255).
	 *
	 * @param col RGBA color value.
	 */
	void set_color(const Eigen::Vector4i col);
	void set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	/**
	 * Set the color used for the skybox using float values
	 * for each channel (0.0 to 1.0).
	 *
	 * @param col RGBA color value.
	 */
	void set_color(const Eigen::Vector4f col);
	void set_color(float r, float g, float b, float a);

	/**
	 * Resize the FBO for the skybox rendering. This basically updates the output
	 * texture size.
	 *
	 * @param width New width of the FBO.
	 * @param height New height of the FBO.
	 */
	void resize(size_t width, size_t height);

private:
	/**
	 * Create the render pass for the skybox.
	 *
	 * Called during initialization of the skybox renderer.
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
	 * Render pass for the skybox drawing.
	 */
	std::shared_ptr<renderer::RenderPass> render_pass;

	/**
	 * Output texture.
	 */
	std::shared_ptr<renderer::Texture2d> output_texture;

	/**
	 * Background color.
	 */
	Eigen::Vector4f bg_color;

	/**
	 * Stores background color uniform for the shader.
	 */
	std::shared_ptr<renderer::UniformInput> color_unif;
};

} // namespace skybox
} // namespace openage::renderer
