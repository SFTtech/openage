// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "renderer/definitions.h"
#include "renderer/renderable.h"


namespace openage {
namespace renderer {
class RenderTarget;

/**
 * Defines a layer in the render pass. A layer is a slice of the renderables
 * that have the same priority. Each layer can have its own settings.
 *
 * // TODO: We could also move these settings to the render pass itself and use
 * //       multiple render passes to achieve the same effect. Then we might
 * //       not need layers at all.
 */
struct Layer {
	/// Priority of the renderables.
	int64_t priority;
	/// Whether to clear the depth buffer before rendering this layer.
	bool clear_depth = true;
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
	 *
	 * @return Renderables of the render pass.
	 */
	const std::vector<std::vector<Renderable>> &get_renderables() const;

	/**
	 * Get the layers of the render pass.
	 *
	 * @return Layers of the render pass.
	 */
	const std::vector<Layer> &get_layers() const;

	/**
	 * Set the render target to write to.
	 *
	 * @param target Render target.
	 */
	void set_target(const std::shared_ptr<RenderTarget> &target);

	/**
	 * Get the render target of the render pass.
	 *
	 * @return Render target.
	 */
	const std::shared_ptr<RenderTarget> &get_target() const;

	/**
	 * Replace the current renderables with the given list of renderables.
	 *
	 * @param renderables New renderables.
	 */
	void set_renderables(std::vector<Renderable> &&renderables);

	/**
	 * Append renderables to the render pass with a given priority.
	 *
	 * @param renderables New renderables.
	 * @param priority Priority of the renderables. Layers with higher priority are drawn later.
	 */
	void add_renderables(std::vector<Renderable> &&renderables,
	                     int64_t priority = LAYER_PRIORITY_MAX);

	/**
	 * Append a single renderable to the render pass with a given priority.
	 *
	 * @param renderable New renderable.
	 * @param priority Priority of the renderable. Layers with higher priority are drawn later.
	 */
	void add_renderables(Renderable &&renderable,
	                     int64_t priority = LAYER_PRIORITY_MAX);

	/**
	 * Add a new layer to the render pass.
	 *
	 * @param priority Priority of the layer. Layers with higher priority are drawn first.
	 * @param clear_depth If true clears the depth buffer before rendering this layer.
	 */
	void add_layer(int64_t priority, bool clear_depth = true);

	/**
	 * Clear the list of renderables
	 */
	void clear_renderables();

	using compare_func = std::function<bool(const Renderable &, const Renderable &)>;

	/**
	 * Sort the renderables using the given comparison function.
	 *
	 * Layers are sorted individually, so the order of layers is not changed.
	 *
	 * @param compare Comparison function.
	 */
	void sort(const compare_func &compare);

protected:
	/**
	 * Create a new RenderPass. This is called from Renderer::add_render_pass,
	 * which then creates the proper subclass of RenderPass, depending on the backend.
	 *
	 * @param renderables The renderables to draw.
	 * @param target Render target to write to.
	 */
	RenderPass(std::vector<Renderable> &&renderables, const std::shared_ptr<RenderTarget> &target);

	/**
	 * The renderables to draw.
	 *
	 * Kept sorted by layer priorities (lowest to highest priority).
	 */
	std::vector<std::vector<Renderable>> renderables;

private:
	/**
	 * Add a new layer to the render pass at the given index.
	 *
	 * @param index Index in \p layers member to insert the new layer.
	 * @param priority Priority of the layer. Layers with higher priority are drawn first.
	 * @param clear_depth If true clears the depth buffer before rendering this layer.
	 */
	void add_layer(size_t index, int64_t priority, bool clear_depth = true);

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
