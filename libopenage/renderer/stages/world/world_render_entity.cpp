// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "world_render_entity.h"

namespace openage::renderer::world {

WorldRenderEntity::WorldRenderEntity() :
	changed{false},
	position{0.0f, 0.0f, 0.0f},
	sprite_path{},
	last_update{0.0} {
}

void WorldRenderEntity::update(const uint32_t ref_id,
                               const coord::phys3 position,
                               const util::Path sprite_path,
                               const curve::time_t time) {
	std::unique_lock lock{this->mutex};

	this->ref_id = ref_id;
	this->position = position.to_scene3();
	this->sprite_path = sprite_path;
	this->changed = true;
	this->last_update = time;
}

uint32_t WorldRenderEntity::get_id() {
	std::shared_lock lock{this->mutex};

	return this->ref_id;
}

const coord::scene3 WorldRenderEntity::get_position() {
	std::shared_lock lock{this->mutex};

	return this->position;
}

const util::Path &WorldRenderEntity::get_texture_path() {
	std::shared_lock lock{this->mutex};

	return this->sprite_path;
}

curve::time_t WorldRenderEntity::get_update_time() {
	std::shared_lock lock{this->mutex};

	return this->last_update;
}

bool WorldRenderEntity::is_changed() {
	std::shared_lock lock{this->mutex};

	return this->changed;
}

void WorldRenderEntity::clear_changed_flag() {
	std::unique_lock lock{this->mutex};

	this->changed = false;
}

} // namespace openage::renderer::world
