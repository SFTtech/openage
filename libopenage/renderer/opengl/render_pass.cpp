// Copyright 2019-2024 the openage authors. See copying.md for legal info.

#include "render_pass.h"

#include "log/log.h"


namespace openage::renderer::opengl {

GlRenderPass::GlRenderPass(std::vector<Renderable> renderables,
                           const std::shared_ptr<RenderTarget> &target) :
	RenderPass(renderables, target),
	is_optimized(false) {
}

void GlRenderPass::set_renderables(std::vector<Renderable> renderables) {
	RenderPass::set_renderables(renderables);
	this->is_optimized = false;
}

bool GlRenderPass::get_is_optimized() const {
	return this->is_optimized;
}

void GlRenderPass::set_is_optimized(bool flag) {
	this->is_optimized = flag;
}
} // namespace openage::renderer::opengl
