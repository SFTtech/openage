// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "render_entity.h"


namespace openage::renderer {

RenderEntity::RenderEntity() :
	changed{false},
	last_update{time::TIME_ZERO},
	fetch_time{time::TIME_MAX} {
}

time::time_t RenderEntity::get_fetch_time() {
	return this->fetch_time;
}

bool RenderEntity::is_changed() {
	std::shared_lock lock{this->mutex};

	return this->changed;
}

void RenderEntity::fetch_done() {
	this->changed = false;
	this->fetch_time = time::TIME_MAX;
}

std::unique_lock<std::shared_mutex> RenderEntity::get_read_lock() {
	return std::unique_lock{this->mutex};
}

} // namespace openage::renderer
