// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "idle.h"

#include "log/log.h"

#include "gamestate/api/ability.h"
#include "gamestate/api/animation.h"
#include "gamestate/api/property.h"
#include "gamestate/component/api/idle.h"
#include "gamestate/game_entity.h"


namespace openage::gamestate::system {

const curve::time_t Idle::idle(const std::shared_ptr<gamestate::GameEntity> &entity,
                               const curve::time_t &start_time) {
	if (not entity->has_component(component::component_t::IDLE)) [[unlikely]] {
		throw Error{ERR << "Entity " << entity->get_id() << " has no idle component."};
	}

	auto idle_component = std::dynamic_pointer_cast<component::Idle>(
		entity->get_component(component::component_t::IDLE));
	auto ability = idle_component->get_ability();
	if (api::APIAbility::check_property(ability, api::property_t::ANIMATED)) {
		auto property = api::APIAbility::get_property(ability, api::property_t::ANIMATED);
		auto animations = api::APIProperty::get_animations(property);
		auto animation_paths = api::APIAnimation::get_animation_paths(animations);

		if (animation_paths.size() > 0) [[likely]] {
			entity->render_update(start_time, animation_paths[0]);
		}
	}

	// TODO: play sound

	return curve::time_t::from_int(0);
}

} // namespace openage::gamestate::system
