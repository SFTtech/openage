// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "render_entity.h"

#include <mutex>


namespace openage::renderer::hud {

HudDragRenderEntity::HudDragRenderEntity(const coord::input drag_start) :
	RenderEntity{},
	drag_pos{nullptr, 0, "", nullptr, drag_start},
	drag_start{drag_start} {
}

void HudDragRenderEntity::update(const coord::input drag_pos,
                                 const time::time_t time) {
	std::unique_lock lock{this->mutex};

	this->drag_pos.set_insert(time, drag_pos);

	this->last_update = time;
	this->changed = true;
}

const curve::Continuous<coord::input> &HudDragRenderEntity::get_drag_pos() {
	return this->drag_pos;
}

const coord::input &HudDragRenderEntity::get_drag_start() {
	return this->drag_start;
}

} // namespace openage::renderer::hud
