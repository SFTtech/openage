// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "util/path.h"

namespace openage::renderer {
class Renderer;
class RenderPass;
class ShaderProgram;
class Texture2d;
class Window;

namespace terrain {
class TerrainRenderEntity;
class TerrainRenderMesh;
class TerrainRenderModel;

/**
 * Manage and render terrain geometry and graphics.
 */
class TerrainRenderer {
public:
	TerrainRenderer(const std::shared_ptr<Window> &window,
	                const std::shared_ptr<renderer::Renderer> &renderer,
	                const util::Path &shaderdir);
	~TerrainRenderer() = default;

	/**
	 * Get the render pass of the terrain renderer.
	 *
	 * @return Render pass for terrain drawing.
	 */
	std::shared_ptr<renderer::RenderPass> get_render_pass();

	/**
	 * Set the current render entity of the terrain renderer.
	 * 
	 * @param render_entity New render entity.
	 */
	void set_render_entity(const std::shared_ptr<TerrainRenderEntity> &entity);

	/**
	 * Update the terrain mesh and texture information.
	 */
	void update();

	/**
	 * Resize the FBO for the terrain rendering. This basically updates the output
     * texture size.
	 *
	 * @param width New width of the FBO.
	 * @param height New height of the FBO.
	 */
	void resize(size_t width, size_t height);

private:
	/**
	 * Create the render pass for terrains.
	 *
	 * Called during initialization of the terrain renderer.
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
	 * Engine interface for updating terrain draw information.
	 */
	std::shared_ptr<TerrainRenderEntity> render_entity;

	/**
	 * 3D model of the terrain.
	 */
	std::shared_ptr<TerrainRenderModel> model;

	/**
	 * Vertex information of the terrain.
	 */
	std::shared_ptr<TerrainRenderMesh> mesh;

	/**
	 * Render pass for the terrain drawing.
	 */
	std::shared_ptr<renderer::RenderPass> render_pass;

	std::shared_ptr<renderer::ShaderProgram> display_shader;

	/**
	 * Output texture.
	 */
	std::shared_ptr<renderer::Texture2d> output_texture;
};

} // namespace terrain
} // namespace openage::renderer