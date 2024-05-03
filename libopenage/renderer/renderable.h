// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>


namespace openage {
namespace renderer {
class Geometry;
class UniformInput;

/**
 * A renderable is a set of a shader UniformInput and a possible draw call.
 * Usually it is one "step" in a RenderPass.
 *
 * The UniformInput only stores the values the CPU at first. When the renderer
 * "executes" the Renderable in a pass, the UniformInput values are uploaded to
 * the shader on the GPU they were created with.
 *
 * If the geometry is nullptr, the uniform values are uploaded to the shader,
 * but no draw call is performed. This can be used to, for example, first set
 * the values of uniforms that many objects have in common, and then only
 * upload the uniforms that vary between them in each draw call. This works
 * because uniform values in any given shader are preserved across a render
 * pass.
 *
 * If geometry is set (i.e. it is not nullptr), the renderer draws the geometry
 * with the shader and other settings in the renderable. The result is written
 * into the render target, defined in the RenderPass.
 */
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

/**
 * Simplified form of Renderable, which is just an update for a shader.
 * When the ShaderUpdate is processed in a RenderPass,
 * the new uniform values (set on the CPU first with unif_in.update(...))
 * are uploaded to the GPU.
 */
struct ShaderUpdate : Renderable {
	ShaderUpdate(std::shared_ptr<UniformInput> const &uniform) :
		Renderable{uniform, nullptr} {}

	ShaderUpdate(std::shared_ptr<UniformInput> &&uniform) :
		Renderable{std::move(uniform), nullptr} {}
};

} // namespace renderer
} // namespace openage
