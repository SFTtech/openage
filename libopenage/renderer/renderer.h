// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <memory>


namespace openage {
namespace renderer {

namespace resources {
	class TextureData;
	class ShaderSource;
}

class ShaderProgram;
class Geometry;
class Texture;
class Renderable;

/// The abstract base for uniform input. Not terribly interesting.
class UniformInput {
protected:
	UniformInput() = default;

public:
	virtual ~UniformInput() = default;
};

/// The abstract base for a render target. Not terribly interesting.
class RenderTarget {
protected:
	RenderTarget() = default;

public:
	virtual ~RenderTarget() = default;
};

/// A render pass is a series of draw calls represented by renderables that output into the given render target.
struct RenderPass {
	/// The renderables to parse and possibly execute.
	std::vector<Renderable> renderables;
	/// The render targe to write into.
	RenderTarget const *target;
};

/// The renderer. This class is used for performing all graphics operations. It is abstract and has implementations
/// for various low-level graphics APIs like OpenGL.
class Renderer {
public:
	virtual ~Renderer() = default;

	/// Uploads the given texture data (usually loaded from disk) to graphics hardware and makes it available
	/// as a Texture object that can be used for various operations.
	virtual std::unique_ptr<Texture> add_texture(resources::TextureData const&) = 0;

	/// Compiles the given shader source code into a shader program. A shader program is the main tool used
	/// for graphics rendering.
	virtual std::unique_ptr<ShaderProgram> add_shader(std::vector<resources::ShaderSource> const&) = 0;

	/// Constructs a render target from the given textures. All subsequent drawing operations pointed at this
	/// target will write to these textures. The textures are attached to the render target according to their
	/// internal format, for example RGB will be attached as a color component and DEPTH will be attached as a
	/// depth component. (TODO depth, not implemented yet)
	virtual std::unique_ptr<RenderTarget> create_texture_target(std::vector<Texture*>) = 0;

	/// Returns the built-in display target that represents the window. Passes pointed at this target will have
	/// their output visible on the screen.
	virtual RenderTarget const* get_display_target() = 0;

	/// Executes a render pass.
	virtual void render(RenderPass const&) = 0;
};

}}
