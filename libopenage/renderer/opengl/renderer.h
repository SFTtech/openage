// Copyright 2017-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "renderer/renderer.h"
#include "util/vector.h"


namespace openage {
namespace renderer {
class RenderPass;
class ShaderProgram;

namespace opengl {
class GlContext;
class GlRenderPass;
class GlRenderTarget;
class GlVertexArray;
class GlWindow;

/// The OpenGL specialization of the rendering interface.
class GlRenderer final : public Renderer {
public:
	/**
	 * Create a new OpenGL renderer.
	 *
	 * @param ctx OpenGL context of the current window.
	 * @param viewport_size Size of the window viewport.
	 * 						Must be adjusted by scale for highDPI displays.
	 */
	GlRenderer(const std::shared_ptr<GlContext> &ctx,
	           const util::Vector2s &viewport_size);

	/**
	 * Add a new texture from existing pixel data.
	 *
	 * @param data Texture data to upload to the GPU.
	 *
	 * @return Created texture object.
	 */
	std::shared_ptr<Texture2d> add_texture(resources::Texture2dData const &data) override;

	/**
	 * Add a new texture from a texture information object.
	 *
	 * @param info Texture information describing texture size and format.
	 *
	 * @return Created texture object.
	 */
	std::shared_ptr<Texture2d> add_texture(resources::Texture2dInfo const &info) override;

	/**
	 * Add a new shader program from shader source code.
	 *
	 * @param srcs Shader source codes to compile into a shader program.
	 *
	 * @return Created shader program.
	 */
	std::shared_ptr<ShaderProgram> add_shader(std::vector<resources::ShaderSource> const &srcs) override;

	/**
	 * Add a new geometry object from existing mesh data.
	 *
	 * Used for complex geometry with vertex attributes.
	 *
	 * @param mesh Mesh data to upload to the GPU.
	 *
	 * @return Created geometry object.
	 */
	std::shared_ptr<Geometry> add_mesh_geometry(resources::MeshData const &mesh) override;

	/**
	 * Add a new geometry object using a bufferless quad.
	 *
	 * Used for drawing a simple quad (rectangle).
	 *
	 * @return Created geometry object.
	 */
	std::shared_ptr<Geometry> add_bufferless_quad() override;

	/**
	 * Add a new render pass.
	 *
	 * Render passes group renderables that are drawn to the same target.
	 *
	 * @param renderables Renderables to be drawn in the pass.
	 * @param target Render target to draw into.
	 *
	 * @return Created render pass.
	 */
	std::shared_ptr<RenderPass> add_render_pass(std::vector<Renderable> renderables,
	                                            const std::shared_ptr<RenderTarget> &target) override;

	/**
	 * Add a render target that draws into the given texture attachments.
	 *
	 * Textures are attached in the order they appear in \p textures (for color attachments).
	 * Make sure to configure \p textures to match the layout of the output in the shader.
	 *
	 * @param textures Textures to attach to the framebuffer.
	 *
	 * @return Created render target.
	 */
	std::shared_ptr<RenderTarget> create_texture_target(std::vector<std::shared_ptr<Texture2d>> const &textures) override;

	/**
	 * Get the render target for displaying on screen, i.e. targetting the window
	 * of the OpenGL context.
	 *
	 * @return Display target.
	 */
	std::shared_ptr<RenderTarget> get_display_target() override;

	/**
	 * Add a new uniform buffer from a uniform buffer information object.
	 *
	 * @param info Uniform buffer information describing the layout of the buffer.
	 *
	 * @return Created uniform buffer.
	 */
	std::shared_ptr<UniformBuffer> add_uniform_buffer(resources::UniformBufferInfo const &) override;

	/**
	 * Add a new uniform buffer from a shader program that has a uniform block.
	 *
	 * @param prog Shader program. The uniform block must be defined in the program.
	 * @param block_name Name of the block in the shader program.
	 *
	 * @return Created uniform buffer.
	 */
	std::shared_ptr<UniformBuffer> add_uniform_buffer(std::shared_ptr<ShaderProgram> const &prog,
	                                                  std::string const &block_name) override;

	/**
	 * Get the current texture output of the display render target, i.e. the
	 * contents of the default framebuffer.
	 *
	 * @return Texture data from the display framebuffer.
	 */
	resources::Texture2dData display_into_data() override;

	/**
	 * Resize the display target to the given size.
	 *
	 * @param width New width.
	 * @param height New height.
	 */
	void resize_display_target(size_t width, size_t height);

	/**
	 * Check whether the graphics backend encountered any errors.
	 */
	void check_error() override;

	/**
	 * Render the given render pass.
	 *
	 * Iterates over the renderables in the pass and draws them to the target.
	 *
	 * @param pass Render pass.
	 */
	void render(const std::shared_ptr<RenderPass> &pass) override;

private:
	/// Optimize the render pass by reordering stuff
	static void optimize(const std::shared_ptr<GlRenderPass> &pass);

	/// The GL context.
	std::shared_ptr<GlContext> gl_context;

	/// The main screen surface as a render target.
	std::shared_ptr<GlRenderTarget> display;

	/// An empty vertex array object (VAO).
	///
	/// This VAO has to be bound at the start of a render pass to ensure
	/// that bufferless quad geometry can be drawn without errors. Drawing a
	/// bufferless quad requires any VAO to be bound
	/// see https://www.khronos.org/opengl/wiki/Vertex_Rendering#Causes_of_rendering_failure
	std::shared_ptr<GlVertexArray> shared_quad_vao;
};

} // namespace opengl
} // namespace renderer
} // namespace openage
