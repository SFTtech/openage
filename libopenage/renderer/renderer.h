// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>


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
class Texture2d;
class UniformBuffer;
class UniformInput;


/// The abstract base for a render target.
class RenderTarget : public std::enable_shared_from_this<RenderTarget> {
protected:
	RenderTarget() = default;

public:
	virtual ~RenderTarget() = default;

	/**
	 * Get an image from the pixels in the render target's framebuffer.
	 *
	 * This should only be called _after_ rendering to the framebuffer has finished.
	 *
	 * @return RGBA texture data.
	 */
	virtual resources::Texture2dData into_data() = 0;

	virtual std::vector<std::shared_ptr<Texture2d>> get_texture_targets() = 0;
};

/// A renderable is a set of a shader UniformInput and a possible draw call.
/// Usually it is one "step" in a RenderPass.
///
/// The UniformInput only stores the values the CPU at first. When the renderer
/// "executes" the Renderable in a pass, the UniformInput values are uploaded to
/// the shader on the GPU they were created with.
///
/// If the geometry is nullptr, the uniform values are uploaded to the shader,
/// but no draw call is performed. This can be used to, for example, first set
/// the values of uniforms that many objects have in common, and then only
/// upload the uniforms that vary between them in each draw call. This works
/// because uniform values in any given shader are preserved across a render
/// pass.
///
/// If geometry is set (i.e. it is not nullptr), the renderer draws the geometry
/// with the shader and other settings in the renderable. The result is written
/// into the render target, defined in the RenderPass.
struct Renderable {
	/// Uniform values to be set in the appropriate shader. Contains a reference
	/// to the correct shader, and this is the shader that will be used for
	/// drawing if geometry is present.
	std::shared_ptr<UniformInput> uniform;
	/// The geometry. It can be a simple primitive or a complex mesh.
	/// Can be nullptr to only set uniforms but do not perform draw call.
	std::shared_ptr<Geometry> geometry;
	/// Whether to perform alpha-based color blending with whatever was in the
	/// render target before.
	bool alpha_blending = true;
	/// Whether to perform depth testing and discard occluded fragments.
	bool depth_test = true;
};


/// Simplified form of Renderable, which is just an update for a shader.
/// When the ShaderUpdate is processed in a RenderPass,
/// the new uniform values (set on the CPU first with unif_in.update(...))
/// are uploaded to the GPU.
struct ShaderUpdate : Renderable {
	ShaderUpdate(std::shared_ptr<UniformInput> const &uniform) :
		Renderable{uniform, nullptr} {}

	ShaderUpdate(std::shared_ptr<UniformInput> &&uniform) :
		Renderable{std::move(uniform), nullptr} {}
};


/// A render pass is a series of draw calls represented by renderables that output into the given render target.
class RenderPass {
protected:
	/// Create a new RenderPass. This is called from Renderer::add_render_pass,
	/// which then creates the proper subclass of RenderPass, depending on the backend.
	RenderPass(std::vector<Renderable>, const std::shared_ptr<RenderTarget> &);
public:
	/// The renderables to parse and possibly execute.
	std::vector<Renderable> renderables;

public:
	virtual ~RenderPass() = default;
	void set_target(const std::shared_ptr<RenderTarget> &);
	const std::shared_ptr<RenderTarget> &get_target() const;

	// Replace the current renderables
	void set_renderables(std::vector<Renderable>);
	// Append renderables to the end of the list of renderables
	void add_renderables(std::vector<Renderable>);
	// Append a single renderable to the end of the list of renderables
	void add_renderables(Renderable);
	// Clear the list of renderables
	void clear_renderables();

private:
	/// The render target to write into.
	std::shared_ptr<RenderTarget> target;
};


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
