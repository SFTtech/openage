// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "render_entity.h"

#include <mutex>


namespace openage::renderer {

RenderEntity::RenderEntity() :
	changed{false},
	last_update{time::time_t::zero()} {
}

time::time_t RenderEntity::get_update_time() {
	std::shared_lock lock{this->mutex};

	return this->last_update;
}

bool RenderEntity::is_changed() {
	std::shared_lock lock{this->mutex};

	return this->changed;
}

void RenderEntity::clear_changed_flag() {
	std::unique_lock lock{this->mutex};

	this->changed = false;
}

std::shared_lock<std::shared_mutex> RenderEntity::get_read_lock() {
	return std::shared_lock{this->mutex};
}

} // namespace openage::renderer
