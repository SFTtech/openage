// Copyright 2019-2023 the openage authors. See copying.md for legal info.

#include "renderer.h"

namespace openage::renderer {

RenderPass::RenderPass(std::vector<Renderable> renderables,
                       const std::shared_ptr<RenderTarget> &target) :
	renderables(std::move(renderables)),
	target{target} {}


const std::shared_ptr<RenderTarget> &RenderPass::get_target() const {
	return this->target;
}


void RenderPass::set_target(const std::shared_ptr<RenderTarget> &target) {
	this->target = target;
}

void RenderPass::set_renderables(std::vector<Renderable> renderables) {
	this->renderables = std::move(renderables);
}
void RenderPass::add_renderables(std::vector<Renderable> renderables) {
	for (auto item : renderables) {
		this->renderables.push_back(item);
	}
}

void RenderPass::add_renderables(Renderable renderable) {
	this->renderables.push_back(std::move(renderable));
}

void RenderPass::clear_renderables() {
	this->renderables.clear();
}

} // namespace openage::renderer
