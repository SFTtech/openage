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
 * Defines a layer in the render pass. A layer is a slice of the renderables
 * that have the same priority. Each layer can have its own settings.
 */
struct Layer {
	/// Priority of the renderables in this slice.
	int64_t priority;
	/// Number of renderables in this slice.
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
	 * Get the renderables of the render pass.
	 */
	const std::vector<Renderable> &get_renderables() const;

	/**
	 * Get the layers of the render pass.
	 */
	const std::vector<Layer> &get_layers() const;

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
	 * Add a new layer to the render pass.
	 *
	 * @param priority Priority of the layer. Layers with higher priority are drawn first.
	 * @param clear_depth Whether to clear the depth buffer before rendering this layer.
	 */
	void add_layer(int64_t priority);

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
	 * Add a new layer to the render pass at the given index.
	 *
	 * @param index Index in \p layers member to insert the new layer.
	 * @param priority Priority of the layer. Layers with higher priority are drawn first.
	 */
	void add_layer(size_t index, int64_t priority);

	/**
	 * Render target to write to.
	 */
	std::shared_ptr<RenderTarget> target;

	/**
	 * Stores the layers of the render pass.
	 *
	 * Layers are slices of the renderables that have the same priority.
	 * They can assign different settings to the renderables in the slice.
	 *
	 * Sorted from lowest to highest priority.
	 */
	std::vector<Layer> layers;
};

} // namespace renderer
} // namespace openage
