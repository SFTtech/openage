// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "renderer/renderable.h"


namespace openage {
namespace renderer {
class RenderTarget;

/**
 * A slice of renderables with the same priority.
 */
struct priority_slice {
	/// The priority of the renderables in this slice.
	int64_t priority;
	/// The number of renderables in this slice.
	size_t length;
};


/**
 * A render pass is a series of draw calls represented by renderables that output
 * into the given render target.
 */
class RenderPass {
public:
	virtual ~RenderPass() = default;

	/**
	 * Set the render target to write to.
	 */
	void set_target(const std::shared_ptr<RenderTarget> &);

	/**
	 * Get the render target of the render pass.
	 */
	const std::shared_ptr<RenderTarget> &get_target() const;

	/**
	 * Replace the current renderables with the given list of renderables.
	 */
	void set_renderables(std::vector<Renderable>);

	/**
	 * Append renderables to the render pass.
	 */
	void add_renderables(std::vector<Renderable>);

	/**
	 * Append a single renderable to the render pass.
	 */
	void add_renderables(Renderable);

	/**
	 * Append renderables to the render pass with a given priority.
	 */
	void add_renderables(std::vector<Renderable>, int64_t priority);

	/**
	 * Append a single renderable to the render pass with a given priority.
	 */
	void add_renderables(Renderable, int64_t priority);

	/**
	 * Clear the list of renderables
	 */
	void clear_renderables();

protected:
	/**
	 * Create a new RenderPass. This is called from Renderer::add_render_pass,
	 * which then creates the proper subclass of RenderPass, depending on the backend.
	 */
	RenderPass(std::vector<Renderable>, const std::shared_ptr<RenderTarget> &);

	/**
	 * The renderables to parse and possibly execute.
	 */
	std::vector<Renderable> renderables;

private:
	/**
	 * Render target to write to.
	 */
	std::shared_ptr<RenderTarget> target;

	/**
	 * Stores the number of renderables in the \p renderables member that
	 * have the same priority.
	 *
	 * The vector is sorted by priority, so the index of the first renderable
	 * with a given priority can be retrieved by adding up the lengths of all
	 * priority slices with a lower priority.
	 */
	std::vector<priority_slice> priority_slices;
};

} // namespace renderer
} // namespace openage
