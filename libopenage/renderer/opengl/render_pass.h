// Copyright 2019-2025 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/opengl/lookup.h"
#include "renderer/render_pass.h"
#include "renderer/renderable.h"


namespace openage::renderer::opengl {

struct StencilConfig {
	bool enabled = false;
	/// Whether to write to stencil buffer
	bool write = false;
	/// Reference value for stencil test
	uint8_t ref_value = 1;
	/// Stencil test function
	GLenum test_func = GL_ALWAYS;
	/// Mask for writing to stencil buffer
	uint8_t write_mask = 0xFF;
	/// Mask for reading from stencil buffer
	uint8_t read_mask = 0xFF;
	/// Stencil operation for stencil fail
	GLenum stencil_fail = GL_KEEP;
	/// Stencil operation for depth fail
	GLenum depth_fail = GL_KEEP;
	/// Stencil operation for pass
	GLenum depth_pass = GL_KEEP;
};

class GlRenderPass final : public RenderPass {
public:
	GlRenderPass(std::vector<Renderable> &&renderables,
	             const std::shared_ptr<RenderTarget> &target);

	void set_renderables(std::vector<Renderable> &&renderables);
	void add_renderables(std::vector<Renderable> &&renderables, int64_t priority = LAYER_PRIORITY_MAX);
	void add_renderables(Renderable &&renderable, int64_t priority = LAYER_PRIORITY_MAX);

	void set_is_optimized(bool);
	bool get_is_optimized() const;

	void set_stencil_config(const StencilConfig config);
	const StencilConfig &get_stencil_config() const;

private:
	/// Whether the renderables order is optimised
	bool is_optimized;

	/// The currentstencil configuration.
	StencilConfig stencil_config;
};

} // namespace openage::renderer::opengl
