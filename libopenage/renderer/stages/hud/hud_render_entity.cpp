// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "hud_render_entity.h"

#include <mutex>


namespace openage::renderer::hud {

HudRenderEntity::HudRenderEntity() :
	changed{false},
	last_update{0.0} {
}

void HudRenderEntity::update(const time::time_t time) {
	std::unique_lock lock{this->mutex};

	// TODO
}

time::time_t HudRenderEntity::get_update_time() {
	std::shared_lock lock{this->mutex};

	return this->last_update;
}

bool HudRenderEntity::is_changed() {
	std::shared_lock lock{this->mutex};

	return this->changed;
}

void HudRenderEntity::clear_changed_flag() {
	std::unique_lock lock{this->mutex};

	this->changed = false;
}

} // namespace openage::renderer::hud
