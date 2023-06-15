// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "game_entity.h"

#include "gamestate/component/api/idle.h"
#include "gamestate/component/api/move.h"
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

GameEntity::GameEntity(entity_id_t id) :
	id{id},
	components{},
	render_entity{nullptr} {
}

std::shared_ptr<GameEntity> GameEntity::copy(entity_id_t id) {
	auto copy = std::shared_ptr<GameEntity>(new GameEntity(*this));
	copy->set_id(id);

	return copy;
}

entity_id_t GameEntity::get_id() const {
	return this->id;
}

void GameEntity::set_render_entity(const std::shared_ptr<renderer::world::WorldRenderEntity> &entity) {
	this->render_entity = entity;

	this->push_to_render();
}

const std::shared_ptr<component::Component> &GameEntity::get_component(component::component_t type) {
	return this->components.at(type);
}

void GameEntity::add_component(const std::shared_ptr<component::Component> &component) {
	this->components.insert({component->get_type(), component});
}

bool GameEntity::has_component(component::component_t type) {
	return this->components.contains(type);
}

void GameEntity::push_to_render() {
	if (this->render_entity != nullptr) {
		if (not this->components.contains(component::component_t::POSITION)) {
			return;
		}

		if (this->components.contains(component::component_t::MOVE)) {
			auto comp = dynamic_pointer_cast<component::Move>(this->components.at(component::component_t::MOVE));
			if (comp->check_property(component::property_t::ANIMATED)) {
				auto property = comp->get_property(component::property_t::ANIMATED);
				auto animations = property.get_set("Animated.animations");

				if (animations.size() < 1) {
					return;
				}

				auto animation = std::dynamic_pointer_cast<nyan::ObjectValue>((*animations.begin()).get_ptr());
				auto db_view = comp->get_ability().get_view();
				auto animation_obj = db_view->get_object(animation->get_name());
				auto anim_path = animation_obj.get_file("Animation.sprite");

				auto obj_path = animation_obj.get_info().get_location().get_file()->get_name();
			}
		}
		else if (this->components.contains(component::component_t::IDLE)) {
			auto comp = dynamic_pointer_cast<component::Idle>(this->components.at(component::component_t::IDLE));
			if (comp->check_property(component::property_t::ANIMATED)) {
				auto property = comp->get_property(component::property_t::ANIMATED);
				auto animations = property.get_set("Animated.animations");

				if (animations.size() < 1) {
					return;
				}

				auto animation = std::dynamic_pointer_cast<nyan::ObjectValue>((*animations.begin()).get_ptr());
				auto db_view = comp->get_ability().get_view();
				auto animation_obj = db_view->get_object(animation->get_name());
				auto anim_path = animation_obj.get_file("Animation.sprite");
			}
		}
		else {
			return;
		}
		return;

		const auto &pos = dynamic_pointer_cast<component::Position>(this->components.at(component::component_t::POSITION))->get_positions();
		const auto &angle = dynamic_pointer_cast<component::Position>(this->components.at(component::component_t::POSITION))->get_angles();
		this->render_entity->update(this->id, pos, angle, this->animation_path);
	}
}

void GameEntity::set_id(entity_id_t id) {
	this->id = id;
}

} // namespace openage::gamestate
