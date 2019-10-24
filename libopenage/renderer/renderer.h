// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <memory>


namespace openage {
namespace renderer {

namespace resources {
class Texture2dData;
class Texture2dInfo;
class ShaderSource;
class MeshData;
}

class ShaderProgram;
class Geometry;
class Texture2d;

/// The abstract base for uniform input. Besides the uniform values, it stores information about
/// which shader program the input was created for.
class UniformInput {
protected:
	UniformInput() = default;

public:
	virtual ~UniformInput() = default;
};

/// The abstract base for a render target.
class RenderTarget {
protected:
	RenderTarget() = default;

public:
	virtual ~RenderTarget() = default;
};

/// A renderable is a set of shader uniform inputs and a possible draw call.
/// When a renderable is parsed, the uniform inputs are first passed to the shader they were created with.
/// Then, if the geometry is not nullptr, the shader is executed with the geometry and the specified settings
/// and the results are written into the render target. If the geometry is nullptr, the uniform values are
/// uploaded to the shader but no draw call is performed. This can be used to, for example, first set the values
/// of uniforms that many objects have in common, and then only upload the uniforms that vary between them in
/// each draw call. This works because uniform values in any given shader are preserved across a render pass.
struct Renderable {
	/// Uniform values to be set in the appropriate shader. Contains a reference to the correct shader, and this
	/// is the shader that will be used for drawing if geometry is present.
	UniformInput const *unif_in;
	/// The geometry. It can be a simple primitive or a complex mesh.
	/// Can be nullptr to only set uniforms but do not perform draw call.
	Geometry const *geometry;
	/// Whether to perform alpha-based color blending with whatever was in the render target before.
	bool alpha_blending;
	/// Whether to perform depth testing and discard occluded fragments.
	bool depth_test;
};

/// A render pass is a series of draw calls represented by renderables that output into the given render target.
class RenderPass {
protected:
	RenderPass(std::vector<Renderable>, RenderTarget const*);
	/// The renderables to parse and possibly execute.
	std::vector<Renderable> renderables;

public:
	virtual ~RenderPass() = default;
	void set_target(RenderTarget const*);
	RenderTarget const* get_target() const;
private:
	/// The render target to write into.
	RenderTarget const *target;
};

/// The renderer. This class is used for performing all graphics operations. It is abstract and has implementations
/// for various low-level graphics APIs like OpenGL.
class Renderer {
public:
	virtual ~Renderer() = default;

	/// Uploads the given texture data (usually loaded from disk) to graphics hardware and makes it available
	/// as a Texture object that can be used for various operations.
	virtual std::unique_ptr<Texture2d> add_texture(resources::Texture2dData const&) = 0;

	/// Creates a new empty texture with the given parameters on the graphics hardware.
	virtual std::unique_ptr<Texture2d> add_texture(resources::Texture2dInfo const&) = 0;

	/// Compiles the given shader source code into a shader program. A shader program is the main tool used
	/// for graphics rendering.
	virtual std::unique_ptr<ShaderProgram> add_shader(std::vector<resources::ShaderSource> const&) = 0;

	/// Creates a Geometry object from the given mesh data, uploading it to the GPU by creating appropriate buffer.
	/// The vertex attributes will be passed to the shader as described in the mesh data.
	virtual std::unique_ptr<Geometry> add_mesh_geometry(resources::MeshData const&) = 0;

	/// Adds a Geometry object that passes a simple 4-vertex drawing command with no vertex attributes to the shader.
	/// Useful for generating positions in the vertex shader.
	virtual std::unique_ptr<Geometry> add_bufferless_quad() = 0;

	/// Creates a render pass object from the given list of renderables that output to the given render target
	virtual std::unique_ptr<RenderPass> add_render_pass(std::vector<Renderable>, RenderTarget const*) = 0;

	/// Constructs a render target from the given textures. All subsequent drawing operations pointed at this
	/// target will write to these textures. Textures are attached to the target in the order in which they
	/// appear within the vector. Depth textures are attached as depth components. Textures of every other
	/// type are attached as color components.
	virtual std::unique_ptr<RenderTarget> create_texture_target(std::vector<Texture2d*>) = 0;

	/// Returns the built-in display target that represents the window. Passes pointed at this target will have
	/// their output visible on the screen.
	virtual RenderTarget const* get_display_target() = 0;

	/// Stores the display framebuffer into a CPU-accessible data object. Essentially, this takes a screenshot.
	virtual resources::Texture2dData display_into_data() = 0;

	/// Executes a render pass.
	/// A renderer implementation might modify the RenderPass.
	virtual void render(RenderPass*) = 0;
};

}}
