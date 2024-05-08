// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "render_pass.h"

#include "log/log.h"


namespace openage::renderer {

RenderPass::RenderPass(std::vector<Renderable> &&renderables,
                       const std::shared_ptr<RenderTarget> &target) :
	renderables{},
	target{target},
	layers{} {
	// Add a default layer with the lowest priority
	this->add_layer(0, LAYER_PRIORITY_MAX);

	// Add the renderables to the pass
	this->add_renderables(std::move(renderables));

	log::log(MSG(dbg) << "Created render pass");
}

const std::vector<Renderable> &RenderPass::get_renderables() const {
	return this->renderables;
}

const std::vector<Layer> &RenderPass::get_layers() const {
	return this->layers;
}

const std::shared_ptr<RenderTarget> &RenderPass::get_target() const {
	return this->target;
}

void RenderPass::set_target(const std::shared_ptr<RenderTarget> &target) {
	this->target = target;
}

void RenderPass::set_renderables(std::vector<Renderable> &&renderables) {
	this->clear_renderables();
	this->add_renderables(std::move(renderables));
}

void RenderPass::add_renderables(std::vector<Renderable> &&renderables, int64_t priority) {
	// Insertion index for the renderables
	size_t renderables_index = 0;

	// Index of the layer where the renderables will be inserted
	size_t layer_index = 0;

	// Priority of the last observed layer
	int64_t current_priority = LAYER_PRIORITY_MAX;

	if (priority == LAYER_PRIORITY_MAX) {
		// Add the renderables to the last (default) layer
		this->renderables.insert(this->renderables.end(),
		                         std::make_move_iterator(renderables.begin()),
		                         std::make_move_iterator(renderables.end()));
		this->layers.back().length += renderables.size();
		return;
	}

	// Find the index in renderables to insert the renderables
	for (size_t i = 0; i < this->layers.size(); i++) {
		auto &layer = this->layers.at(i);
		if (layer.priority > priority) {
			// Priority of the next layer is lower than the desired priority
			// Insert the renderables directly before this layer
			break;
		}
		renderables_index += layer.length;
		current_priority = layer.priority;
		layer_index = i;
	}

	this->renderables.insert(this->renderables.begin() + renderables_index,
	                         std::make_move_iterator(renderables.begin()),
	                         std::make_move_iterator(renderables.end()));

	if (current_priority != priority) {
		// Lazily add a new layer with the desired priority
		this->add_layer(layer_index, priority);
	}

	// Update the length of the layer with the number of added renderables
	this->layers.at(layer_index).length += renderables.size();
}

void RenderPass::add_renderables(Renderable &&renderable, int64_t priority) {
	this->add_renderables(std::vector<Renderable>{std::move(renderable)}, priority);
}

void RenderPass::add_layer(int64_t priority, bool clear_depth) {
	size_t layer_index = 0;
	for (const auto &layer : this->layers) {
		if (layer.priority > priority) {
			break;
		}
		layer_index++;
	}

	this->add_layer(layer_index, priority, clear_depth);
}

void RenderPass::add_layer(size_t index, int64_t priority, bool clear_depth) {
	this->layers.insert(this->layers.begin() + index, Layer{priority, 0, clear_depth});
}

void RenderPass::clear_renderables() {
	// Erase all renderables
	this->renderables.clear();

	// Keep layer definitions, but reset the length of each layer to 0
	for (auto &layer : this->layers) {
		layer.length = 0;
	}
}

void RenderPass::sort(const compare_func &compare) {
	size_t offset = 0;
	for (auto &layer : this->layers) {
		std::stable_sort(this->renderables.begin() + offset,
		                 this->renderables.begin() + offset + layer.length,
		                 compare);
		offset += layer.length;
	}
}

} // namespace openage::renderer
