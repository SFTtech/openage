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

	std::shared_ptr<Texture2d> add_texture(resources::Texture2dData const &) override;
	std::shared_ptr<Texture2d> add_texture(resources::Texture2dInfo const &) override;

	std::shared_ptr<ShaderProgram> add_shader(std::vector<resources::ShaderSource> const &) override;

	std::shared_ptr<Geometry> add_mesh_geometry(resources::MeshData const &) override;
	std::shared_ptr<Geometry> add_bufferless_quad() override;

	std::shared_ptr<RenderPass> add_render_pass(std::vector<Renderable>, const std::shared_ptr<RenderTarget> &) override;

	std::shared_ptr<RenderTarget> create_texture_target(std::vector<std::shared_ptr<Texture2d>> const &) override;

	std::shared_ptr<RenderTarget> get_display_target() override;

	std::shared_ptr<UniformBuffer> add_uniform_buffer(resources::UniformBufferInfo const &) override;
	std::shared_ptr<UniformBuffer> add_uniform_buffer(std::shared_ptr<ShaderProgram> const &,
	                                                  std::string const &) override;

	resources::Texture2dData display_into_data() override;

	void resize_display_target(size_t width, size_t height);

	void check_error() override;

	void render(const std::shared_ptr<RenderPass> &) override;

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
