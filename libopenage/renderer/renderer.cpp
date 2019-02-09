// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#include "renderer.h"

namespace openage::renderer {

RenderPass::RenderPass(std::vector<Renderable> renderables,
                       const std::shared_ptr<RenderTarget> &target)
	:
	renderables(std::move(renderables)),
	target{target} {}


const std::shared_ptr<RenderTarget> &RenderPass::get_target() const {
	return this->target;
}


void RenderPass::set_target(const std::shared_ptr<RenderTarget> &target) {
	this->target = target;
}

} // namespace openage::renderer
