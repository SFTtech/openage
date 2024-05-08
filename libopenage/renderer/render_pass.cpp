// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "render_pass.h"

#include <limits>


namespace openage::renderer {

RenderPass::RenderPass(std::vector<Renderable> renderables,
                       const std::shared_ptr<RenderTarget> &target) :
	renderables(std::move(renderables)),
	target{target},
	layers{} {
	// Add a default layer with the lowest priority
	this->add_layer(0, std::numeric_limits<int64_t>::max());
}

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
	this->renderables.insert(this->renderables.end(), renderables.begin(), renderables.end());

	this->layers.front().length += renderables.size();
}

void RenderPass::add_renderables(Renderable renderable) {
	this->add_renderables(std::vector<Renderable>{std::move(renderable)});
}

void RenderPass::add_renderables(std::vector<Renderable> renderables, int64_t priority) {
	size_t renderables_index = 0;
	size_t layer_index = 0;
	int64_t current_priority = std::numeric_limits<int64_t>::min();
	for (size_t i = 0; i < this->layers.size(); i++) {
		auto &layer = this->layers.at(i);
		if (layer.priority < priority) {
			break;
		}
		renderables_index += layer.length;
		current_priority = layer.priority;
		layer_index = i;
	}

	this->renderables.insert(this->renderables.begin() + renderables_index, renderables.begin(), renderables.end());

	if (current_priority != priority) {
		layer_index += 1;
		this->add_layer(layer_index, priority);
	}

	this->layers.at(layer_index).length += renderables.size();
}

void RenderPass::add_renderables(Renderable renderable, int64_t priority) {
	this->add_renderables(std::vector<Renderable>{std::move(renderable)}, priority);
}

void RenderPass::add_layer(int64_t priority) {
	size_t layer_index = 0;
	for (const auto &layer : this->layers) {
		if (layer.priority < priority) {
			break;
		}
		layer_index++;
	}

	this->add_layer(layer_index, priority);
}

const std::vector<Renderable> &RenderPass::get_renderables() const {
	return this->renderables;
}

const std::vector<Layer> &RenderPass::get_layers() const {
	return this->layers;
}

void RenderPass::add_layer(size_t index, int64_t priority) {
	this->layers.insert(this->layers.begin() + index, Layer{priority, 0});
}

void RenderPass::clear_renderables() {
	this->renderables.clear();
}

} // namespace openage::renderer
