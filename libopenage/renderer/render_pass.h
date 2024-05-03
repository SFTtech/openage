// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

#include "renderer/renderable.h"


namespace openage {
namespace renderer {
class RenderTarget;

/// A render pass is a series of draw calls represented by renderables that output into the given render target.
class RenderPass {
protected:
	/// Create a new RenderPass. This is called from Renderer::add_render_pass,
	/// which then creates the proper subclass of RenderPass, depending on the backend.
	RenderPass(std::vector<Renderable>, const std::shared_ptr<RenderTarget> &);
	/// The renderables to parse and possibly execute.
	std::vector<Renderable> renderables;

public:
	virtual ~RenderPass() = default;
	void set_target(const std::shared_ptr<RenderTarget> &);
	const std::shared_ptr<RenderTarget> &get_target() const;

	// Replace the current renderables
	void set_renderables(std::vector<Renderable>);
	// Append renderables to the end of the list of renderables
	void add_renderables(std::vector<Renderable>);
	// Append a single renderable to the end of the list of renderables
	void add_renderables(Renderable);
	// Clear the list of renderables
	void clear_renderables();

private:
	/// The render target to write into.
	std::shared_ptr<RenderTarget> target;
};

} // namespace renderer
} // namespace openage
