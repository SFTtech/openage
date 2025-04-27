// Copyright 2019-2025 the openage authors. See copying.md for legal info.

#include "render_pass.h"

#include "log/log.h"


namespace openage::renderer::opengl {

GlRenderPass::GlRenderPass(std::vector<Renderable> &&renderables,
                           const std::shared_ptr<RenderTarget> &target) :
	RenderPass(std::move(renderables), target),
	is_optimized(false) {
}

void GlRenderPass::set_renderables(std::vector<Renderable> &&renderables) {
	RenderPass::set_renderables(std::move(renderables));
	this->is_optimized = false;
}

void GlRenderPass::add_renderables(std::vector<Renderable> &&renderables, int64_t priority) {
	RenderPass::add_renderables(std::move(renderables), priority);
	this->is_optimized = false;
}

void GlRenderPass::add_renderables(Renderable &&renderable, int64_t priority) {
	RenderPass::add_renderables(std::move(renderable), priority);
	this->is_optimized = false;
}

bool GlRenderPass::get_is_optimized() const {
	return this->is_optimized;
}

void GlRenderPass::set_is_optimized(bool flag) {
	this->is_optimized = flag;
}

void GlRenderPass::set_stencil_config(const StencilConfig config) {
	this->stencil_config = config;
}

const StencilConfig &GlRenderPass::get_stencil_config() const {
	return this->stencil_config;
}

} // namespace openage::renderer::opengl
