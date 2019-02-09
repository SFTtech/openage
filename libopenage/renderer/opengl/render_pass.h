// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#pragma once

#include "../renderer.h"

namespace openage::renderer::opengl {

class GlRenderPass final : public RenderPass {
public:
	GlRenderPass(std::vector<Renderable>,
	             const std::shared_ptr<RenderTarget> &);

	void set_renderables(std::vector<Renderable>);
	std::vector<Renderable> get_renderables() const;
	void set_is_optimised(bool);
	bool get_is_optimised() const;

private:
	/// Whether the renderables order is optimised
	bool is_optimised;
};

} // openage::renderer::opengl
