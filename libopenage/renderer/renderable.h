// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <vector>
#include <utility>
#include <mutex>

namespace openage {
namespace renderer {

class UniformInput;
class Geometry;

/// A renderable is a set of shader uniform inputs and a possible draw call.
/// When a renderable is parsed, the uniform inputs are first passed to the shader they were created with.
/// Then, if the geometry is not nullptr, the shader is executed with the geometry and the specified settings
/// and the results are written into the render target. If the geometry is nullptr, the uniform values are
/// uploaded to the shader but no draw call is performed. This can be used to, for example, first set the values
/// of uniforms that many objects have in common, and then only upload the uniforms that vary between them in
/// each draw call. This works because uniform values in any given shader are preserved across a render pass.
class Renderable {
public:
	using function_t = std::function<void(void)>;
	using id_t = uint16_t;

	Renderable(UniformInput const *uniform_input = nullptr, Geometry const* geometry = nullptr, const bool alpha_blending = true, const bool depth_test = true,
			const bool writes_id = true, const function_t &on_click = [](){}, const function_t &on_area_select = [](){});
	Renderable(const Renderable& renderable);
	~Renderable();

	Renderable& operator=(const Renderable& renderable);
	Renderable& operator=(const Renderable&& renderable) = delete;

	static Renderable* get(const id_t id);

	/// Uniform values to be set in the appropriate shader.
	UniformInput const *unif_in;
	/// The geometry. It can be a simple primitive or a complex mesh.
	/// Can be nullptr to only set uniforms but do not perform draw call.
	Geometry const *geometry;
	/// Whether to perform alpha-based color blending with whatever was in the render target before.
	bool alpha_blending;
	/// Whether to perform depth testing and discard occluded fragments.
	bool depth_test;

	const id_t id;

	bool writes_id;

	function_t on_click_callback;
	function_t on_area_selected_callback;

private:
	static id_t free_id();

	static std::vector<Renderable*> RENDERABLES;
	static std::vector<id_t> FREE_IDS;

	static std::mutex ID_MUTEX;
};

}}
