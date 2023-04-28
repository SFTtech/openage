// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "game_entity.h"

#include "gamestate/component/base_component.h"
#include "gamestate/component/internal/position.h"
#include "renderer/stages/world/world_render_entity.h"

namespace openage::gamestate {

GameEntity::GameEntity(entity_id_t id,
                       coord::phys3 &initial_pos,
                       util::Path &texture_path) :
	id{id},
	pos{initial_pos},
	texture_path{texture_path},
	components{},
	render_entity{nullptr} {
}

entity_id_t GameEntity::get_id() const {
	return this->id;
}

void GameEntity::set_render_entity(const std::shared_ptr<renderer::world::WorldRenderEntity> &entity) {
	this->render_entity = entity;

	this->push_to_render();
}

void GameEntity::add_component(const std::shared_ptr<component::Component> &component) {
	this->components.emplace(component->get_component_type(), component);
}

void GameEntity::push_to_render() {
	if (this->render_entity != nullptr) {
		this->render_entity->update(this->id,
		                            this->pos,
		                            this->texture_path);
	}
}

} // namespace openage::gamestate
