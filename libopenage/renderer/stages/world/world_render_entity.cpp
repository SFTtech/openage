// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "world_render_entity.h"

namespace openage::renderer::world {

WorldRenderEntity::WorldRenderEntity() :
	changed{false},
	position{0.0f, 0.0f, 0.0f},
	texture_path{} {
}

void WorldRenderEntity::update(const util::Vector3f position,
                               const util::Path texture_path) {
	this->position = position;
	this->texture_path = texture_path;
	this->changed = true;
}

const util::Vector3f WorldRenderEntity::get_position() {
	return this->position;
}

const util::Path &WorldRenderEntity::get_texture_path() {
	return this->texture_path;
}

bool WorldRenderEntity::is_changed() {
	return this->changed;
}

void WorldRenderEntity::clear_changed_flag() {
	this->changed = false;
}

} // namespace openage::renderer::world