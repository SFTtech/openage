// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "render_entity.h"

#include <mutex>


namespace openage::renderer::hud {

DragRenderEntity::DragRenderEntity(const coord::input drag_start) :
	renderer::RenderEntity{},
	drag_pos{nullptr, 0, "", nullptr, drag_start},
	drag_start{drag_start} {
}

void DragRenderEntity::update(const coord::input drag_pos,
                              const time::time_t time) {
	std::unique_lock lock{this->mutex};

	this->drag_pos.set_insert(time, drag_pos);

	this->last_update = time;
	this->changed = true;
}

const curve::Continuous<coord::input> &DragRenderEntity::get_drag_pos() {
	std::shared_lock lock{this->mutex};

	return this->drag_pos;
}

const coord::input DragRenderEntity::get_drag_start() {
	std::shared_lock lock{this->mutex};

	return this->drag_start;
}

} // namespace openage::renderer::hud
