// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "game_entity.h"

#include "gamestate/component/base_component.h"
#include "gamestate/component/internal/position.h"
#include "renderer/stages/world/world_render_entity.h"

namespace openage::gamestate {

GameEntity::GameEntity(entity_id_t id,
                       util::Path &animation_path) :
	id{id},
	animation_path{animation_path},
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
	this->components.insert({component->get_type(), component});
}

void GameEntity::push_to_render() {
	if (this->render_entity != nullptr) {
		if (not this->components.contains(component::component_t::POSITION)) {
			return;
		}

		const auto &pos = dynamic_pointer_cast<component::Position>(this->components.at(component::component_t::POSITION))->get_positions();
		this->render_entity->update(this->id, pos, this->animation_path);
	}
}

} // namespace openage::gamestate
