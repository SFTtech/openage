// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <vector>
#include <memory>


namespace openage {
namespace renderer {

namespace resources {
	class TextureData;
	class ShaderSource;
}

// In terms of the graphics pipeline, there is no difference between APIs. The difference can be contained internally.
// ShaderProgram is API-independent.
class ShaderProgram;
class Geometry;
class Texture;

class UniformInput {
protected:
	UniformInput() {}

public:
	virtual ~UniformInput() {}
};

class RenderTarget {
protected:
	RenderTarget() {}

public:
	virtual ~RenderTarget() {}
};


// each renderable shoud be UniformInput, Geometry, State
// Geometry could be Billboard, Mesh, for now restrict to Billboard
// each object is a shader valuation + execution of draw with settings and optional mesh
// instancing for same shader & mesh
struct Renderable {
	UniformInput const *unif_in;
	// can be nullptr to only set uniforms but do not perform draw call
	Geometry const *geometry;
	bool alpha_blending;
	bool depth_test;
	bool back_culling;
	bool front_clockwise;
};

// what's a render pass? a series of draw calls that output into a texture
// it's a function similarly to a shader
// this time, inputs are shader invocations (with gl state vars such as blending for each invocation)
struct RenderPass {
	std::vector<Renderable> renderables;
	RenderTarget const *target;
	float default_depth;
	uint8_t msaa_level;
};

class Renderer {
protected:
	Renderer() {}

public:
	virtual ~Renderer() = default;

	virtual std::unique_ptr<Texture> add_texture(resources::TextureData const&) = 0;

	virtual std::unique_ptr<ShaderProgram> add_shader(std::vector<resources::ShaderSource> const&) = 0;

	virtual std::unique_ptr<RenderTarget> create_texture_target(Texture const*) = 0;
	virtual RenderTarget const* get_framebuffer_target() = 0;

	virtual void render(RenderPass const&) = 0;
};

}}
