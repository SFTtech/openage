// Copyright 2019-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/render_pass.h"
#include "renderer/renderable.h"


namespace openage::renderer::opengl {

class GlRenderPass final : public RenderPass {
public:
	GlRenderPass(std::vector<Renderable> &&renderables,
	             const std::shared_ptr<RenderTarget> &target);

	void set_renderables(std::vector<Renderable> &&renderables);
	void add_renderables(std::vector<Renderable> &&renderables, int64_t priority = LAYER_PRIORITY_MAX);
	void add_renderables(Renderable &&renderable, int64_t priority = LAYER_PRIORITY_MAX);

	void set_is_optimized(bool);
	bool get_is_optimized() const;

private:
	/// Whether the renderables order is optimised
	bool is_optimized;
};

} // namespace openage::renderer::opengl
