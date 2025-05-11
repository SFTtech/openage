// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#include "target_in_range.h"

#include <nyan/nyan.h>

#include "gamestate/api/ability.h"
#include "gamestate/component/internal/command_queue.h"
#include "gamestate/component/internal/position.h"
#include "gamestate/component/types.h"
#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"


namespace openage::gamestate::activity {

bool target_in_range(const time::time_t &time,
                     const std::shared_ptr<gamestate::GameEntity> &entity,
                     const std::shared_ptr<gamestate::GameState> &state,
                     const std::shared_ptr<nyan::Object> &condition) {
	auto command_queue = std::dynamic_pointer_cast<gamestate::component::CommandQueue>(
		entity->get_component(component::component_t::COMMANDQUEUE));
	auto target = command_queue->get_target().get(time);

	if (not target.has_value()) {
		// No target exists, exit early
		return false;
	}

	auto ability_obj = condition->get<nyan::Object>("NextCommand.ability");
    auto component_type = api::APIAbility::get_component_type(*ability_obj);
    if (not entity->has_component(component_type)) {
        // Entity does not have the component matching the ability, exit early
        return false;
    }

	// Fetch min/max range from the ability
	nyan::value_float_t min_range = 0;
	nyan::value_float_t max_range = 0;
	if (api::APIAbility::check_property(*ability_obj, api::ability_property_t::RANGED)) {
		// Get min/max range from the property of the ability
		auto range_obj = api::APIAbility::get_property(*ability_obj, api::ability_property_t::RANGED);
		min_range = range_obj.get_float("Ranged.min_range");
		max_range = range_obj.get_float("Ranged.max_range");
	}

	// Fetch the current position of the entity
	auto position = std::dynamic_pointer_cast<gamestate::component::Position>(
		entity->get_component(component::component_t::POSITION));
	auto current_pos = position->get_positions().get(time);

	if (std::holds_alternative<coord::phys3>(target.value())) {
		// Target is a position
		auto target_pos = std::get<coord::phys3>(target.value());
		auto distance = (target_pos - current_pos).length();

		return (distance >= min_range and distance <= max_range);
	}

	// Target is a game entity
	auto target_entity_id = std::get<entity_id_t>(target.value());
    auto target_entity = state->get_game_entity(target_entity_id);

    auto target_position = std::dynamic_pointer_cast<gamestate::component::Position>(
        target_entity->get_component(component::component_t::POSITION));
    auto target_pos = target_position->get_positions().get(time);
    auto distance = (target_pos - current_pos).length();

    return (distance >= min_range and distance <= max_range);
}

} // namespace openage::gamestate::activity
