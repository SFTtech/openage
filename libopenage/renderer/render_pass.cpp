// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "render_pass.h"

#include <limits>


namespace openage::renderer {

RenderPass::RenderPass(std::vector<Renderable> renderables,
                       const std::shared_ptr<RenderTarget> &target) :
	renderables(std::move(renderables)),
	target{target} {}


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

	if (this->priority_slices.empty() or this->priority_slices.back().priority != std::numeric_limits<int64_t>::max()) {
		this->priority_slices.push_back(priority_slice{std::numeric_limits<int64_t>::max(), renderables.size()});
		return;
	}

	this->priority_slices.back().length += renderables.size();
}

void RenderPass::add_renderables(Renderable renderable) {
	this->renderables.push_back(std::move(renderable));

	if (this->priority_slices.empty() or this->priority_slices.back().priority != std::numeric_limits<int64_t>::max()) {
		this->priority_slices.push_back(priority_slice{std::numeric_limits<int64_t>::max(), 1});
		return;
	}

	this->priority_slices.back().length += 1;
}

void RenderPass::add_renderables(std::vector<Renderable> renderables, int64_t priority) {
	size_t renderables_index = 0;
	size_t slice_index = 0;
	int64_t current_priority = std::numeric_limits<int64_t>::min();
	for (auto i = 0; i < this->priority_slices.size(); i++) {
		auto &slice = this->priority_slices.at(i);
		if (slice.priority > priority) {
			slice_index = i;
			break;
		}
		renderables_index += slice.length;
		current_priority = slice.priority;
	}

	this->renderables.insert(this->renderables.begin() + renderables_index, renderables.begin(), renderables.end());

	if (current_priority == priority) {
		this->priority_slices[slice_index].length += renderables.size();
	}
	else {
		this->priority_slices.insert(this->priority_slices.begin() + slice_index, priority_slice{priority, renderables.size()});
	}
}

void RenderPass::add_renderables(Renderable renderable, int64_t priority) {
	size_t renderables_index = 0;
	size_t slice_index = 0;
	int64_t current_priority = std::numeric_limits<int64_t>::min();
	for (auto i = 0; i < this->priority_slices.size(); i++) {
		auto &slice = this->priority_slices.at(i);
		if (slice.priority > priority) {
			slice_index = i;
			break;
		}
		renderables_index += slice.length;
		current_priority = slice.priority;
	}

	this->renderables.insert(this->renderables.begin() + renderables_index, std::move(renderable));

	if (current_priority == priority) {
		this->priority_slices[slice_index].length += 1;
	}
	else {
		this->priority_slices.insert(this->priority_slices.begin() + slice_index, priority_slice{priority, 1});
	}
}

void RenderPass::clear_renderables() {
	this->renderables.clear();
}

} // namespace openage::renderer
