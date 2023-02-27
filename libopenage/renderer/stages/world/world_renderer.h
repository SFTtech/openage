// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <shared_mutex>
#include <vector>

#include "util/path.h"

namespace openage::renderer {
class Renderer;
class RenderPass;
class ShaderProgram;
class Texture2d;
class Window;

namespace resources {
class AssetManager;
}

namespace world {
class WorldRenderEntity;
class WorldObject;

/**
 * Renderer for drawing and displaying entities in the game world (units, buildings, etc.)
 */
class WorldRenderer {
public:
	WorldRenderer(const std::shared_ptr<Window> &window,
	              const std::shared_ptr<renderer::Renderer> &renderer,
	              const util::Path &shaderdir,
	              const std::shared_ptr<renderer::resources::AssetManager> &asset_manager);
	~WorldRenderer() = default;

	/**
	 * Get the render pass of the world renderer.
	 *
	 * @return Render pass for world drawing.
	 */
	std::shared_ptr<renderer::RenderPass> get_render_pass();

	/**
	 * Add a new render entity of the world renderer.
	 *
	 * @param render_entity New render entity.
	 */
	void add_render_entity(const std::shared_ptr<WorldRenderEntity> entity);

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
	 * Texture manager for loading assets.
	 */
	std::shared_ptr<renderer::resources::AssetManager> asset_manager;

	/**
	 * Render pass for the world drawing.
	 */
	std::shared_ptr<renderer::RenderPass> render_pass;

	/**
	 * Render entities requested by the game world.
	 */
	std::vector<std::shared_ptr<WorldObject>> render_objects;

	/**
	 * Shader for rendering the world objects.
	 */
	std::shared_ptr<renderer::ShaderProgram> display_shader;

	/**
	 * Output texture.
	 */
	std::shared_ptr<renderer::Texture2d> output_texture;

	/**
	 * ID texture.
	 */
	std::shared_ptr<renderer::Texture2d> id_texture;

	/**
	 * Mutex for protecting threaded access.
	 */
	std::shared_mutex mutex;
};
} // namespace world
} // namespace openage::renderer
