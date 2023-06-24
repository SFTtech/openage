// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "idle.h"

#include "gamestate/api/ability.h"
#include "gamestate/api/animation.h"
#include "gamestate/api/property.h"
#include "gamestate/component/api/idle.h"
#include "gamestate/game_entity.h"


namespace openage::gamestate::system {

void Idle::idle(const std::shared_ptr<gamestate::GameEntity> &entity,
                const curve::time_t &start_time) {
	auto idle_component = std::dynamic_pointer_cast<component::Idle>(
		entity->get_component(component::component_t::IDLE));
	auto ability = idle_component->get_ability();
	if (api::APIAbility::check_property(ability, api::property_t::ANIMATED)) {
		auto property = api::APIAbility::get_property(ability, api::property_t::ANIMATED);
		auto animations = api::APIProperty::get_animations(property);
		auto animation_paths = api::APIAnimation::get_animation_paths(animations);

		if (animation_paths.size() < 1) {
			return;
		}
		entity->set_animation_path(animation_paths.at(0));
		entity->push_to_render();
	}

	// TODO: play sound
}

} // namespace openage::gamestate::system
