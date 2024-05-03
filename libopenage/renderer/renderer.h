// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "renderer/renderable.h"


namespace openage {
namespace renderer {

namespace resources {
class Texture2dData;
class Texture2dInfo;
class ShaderSource;
class MeshData;
class UniformBufferInfo;
} // namespace resources

class ShaderProgram;
class Geometry;
class RenderPass;
class RenderTarget;
class Texture2d;
class UniformBuffer;
class UniformInput;


/// The renderer. This class is used for performing all graphics operations. It is abstract and has implementations
/// for various low-level graphics APIs like OpenGL.
class Renderer {
public:
	virtual ~Renderer() = default;

	/// Uploads the given texture data (usually loaded from disk) to graphics hardware and makes it available
	/// as a Texture object that can be used for various operations.
	virtual std::shared_ptr<Texture2d> add_texture(resources::Texture2dData const &) = 0;

	/// Creates a new empty texture with the given parameters on the graphics hardware.
	virtual std::shared_ptr<Texture2d> add_texture(resources::Texture2dInfo const &) = 0;

	/// Compiles the given shader source code into a shader program. A shader program is the main tool used
	/// for graphics rendering.
	virtual std::shared_ptr<ShaderProgram> add_shader(std::vector<resources::ShaderSource> const &) = 0;

	/// Creates a Geometry object from the given mesh data, uploading it to the GPU by creating appropriate buffer.
	/// The vertex attributes will be passed to the shader as described in the mesh data.
	virtual std::shared_ptr<Geometry> add_mesh_geometry(resources::MeshData const &) = 0;

	/// Adds a Geometry object that passes a simple 4-vertex drawing command with no vertex attributes to the shader.
	/// Useful for generating positions in the vertex shader.
	virtual std::shared_ptr<Geometry> add_bufferless_quad() = 0;

	/// Creates a render pass object from the given list of renderables that output to the given render target
	virtual std::shared_ptr<RenderPass> add_render_pass(std::vector<Renderable>,
	                                                    const std::shared_ptr<RenderTarget> &) = 0;

	/// Constructs a render target from the given textures. All subsequent drawing operations pointed at this
	/// target will write to these textures. Textures are attached to the target in the order in which they
	/// appear within the vector. Depth textures are attached as depth components. Textures of every other
	/// type are attached as color components.
	virtual std::shared_ptr<RenderTarget> create_texture_target(std::vector<std::shared_ptr<Texture2d>> const &) = 0;

	/// Returns the built-in display target that represents the window. Passes pointed at this target will have
	/// their output visible on the screen.
	virtual std::shared_ptr<RenderTarget> get_display_target() = 0;

	/// Creates a new uniform buffer. Uniform buffers to set uniforms across shaders invocations.
	virtual std::shared_ptr<UniformBuffer> add_uniform_buffer(resources::UniformBufferInfo const &) = 0;

	/// Creates a new uniform buffer from a uniform block in a shader.
	virtual std::shared_ptr<UniformBuffer> add_uniform_buffer(std::shared_ptr<ShaderProgram> const &,
	                                                          std::string const &) = 0;

	/// Stores the display framebuffer into a CPU-accessible data object. Essentially, this takes a screenshot.
	virtual resources::Texture2dData display_into_data() = 0;

	/// Runs error checking code on the current renderer state.
	virtual void check_error() = 0;

	/// Executes a render pass.
	/// A renderer implementation might modify the RenderPass because of optimizations.
	virtual void render(const std::shared_ptr<RenderPass> &) = 0;
};

} // namespace renderer
} // namespace openage
