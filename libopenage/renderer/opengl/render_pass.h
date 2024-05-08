// Copyright 2019-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/render_pass.h"
#include "renderer/renderable.h"


namespace openage::renderer::opengl {

class GlRenderPass final : public RenderPass {
public:
	GlRenderPass(std::vector<Renderable>,
	             const std::shared_ptr<RenderTarget> &);

	void set_renderables(std::vector<Renderable>);
	void set_is_optimized(bool);
	bool get_is_optimized() const;

private:
	/// Whether the renderables order is optimised
	bool is_optimized;
};

} // namespace openage::renderer::opengl
