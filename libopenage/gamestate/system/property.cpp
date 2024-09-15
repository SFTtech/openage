// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "property.h"

#include "gamestate/api/ability.h"
#include "gamestate/api/animation.h"
#include "gamestate/api/property.h"
#include "gamestate/game_entity.h"


namespace openage::gamestate::system {

bool handle_animated(const std::shared_ptr<gamestate::GameEntity> &entity,
                     const nyan::Object &ability,
                     const time::time_t &start_time) {
	bool animated = api::APIAbility::check_property(ability, api::ability_property_t::ANIMATED);

	if (animated) {
		auto property = api::APIAbility::get_property(ability, api::ability_property_t::ANIMATED);
		auto animations = api::APIAbilityProperty::get_animations(property);
		auto animation_paths = api::APIAnimation::get_animation_paths(animations);

		if (animation_paths.size() > 0) [[likely]] {
			// TODO: More than one animation path
			entity->render_update(start_time, animation_paths[0]);
		}
	}

	return animated;
}

} // namespace openage::gamestate::system
