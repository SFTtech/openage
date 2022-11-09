// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "util/path.h"

namespace openage::renderer {
class Renderer;
class RenderPass;
class Window;

namespace terrain {

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
	 * @param shaderdir Directory containg the shader source files.
	 */
	void initialize_render_pass(const util::Path &shaderdir);

	// Interface for the engine
	//std::shared_ptr<TerrainRenderEntity> render_entity;

	// Vertex mesh data for terrain geometry
	//std::shared_ptr<TerrainTextureMap> texture_map;

	// Vertex mesh data for terrain geometry
	//std::shared_ptr<TerrainMesh> mesh;

	/**
	 * Render pass for the terrain drawing.
	 */
	std::shared_ptr<renderer::RenderPass> render_pass;

	/**
	 * Reference to the openage renderer.
	 */
	std::shared_ptr<renderer::Renderer> renderer;
};

} // namespace terrain
} // namespace openage::renderer