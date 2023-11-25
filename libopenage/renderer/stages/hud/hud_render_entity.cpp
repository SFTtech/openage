// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "hud_render_entity.h"

#include <mutex>


namespace openage::renderer::hud {

HudDragRenderEntity::HudDragRenderEntity(const coord::input drag_start) :
	changed{false},
	last_update{0.0},
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

time::time_t HudDragRenderEntity::get_update_time() {
	std::shared_lock lock{this->mutex};

	return this->last_update;
}

const curve::Continuous<coord::input> &HudDragRenderEntity::get_drag_pos() {
	return this->drag_pos;
}

const coord::input &HudDragRenderEntity::get_drag_start() {
	return this->drag_start;
}

bool HudDragRenderEntity::is_changed() {
	std::shared_lock lock{this->mutex};

	return this->changed;
}

void HudDragRenderEntity::clear_changed_flag() {
	std::unique_lock lock{this->mutex};

	this->changed = false;
}

} // namespace openage::renderer::hud
