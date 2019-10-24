// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#include "renderer.h"

namespace openage::renderer {

RenderPass::RenderPass(std::vector<Renderable> renderables, RenderTarget const* target)
:renderables(renderables), target(target) {}

RenderTarget const* RenderPass::get_target() const {
	return this->target;
}

void RenderPass::set_target(RenderTarget const* target) {
	this->target = target;
}

} // openage::renderer
