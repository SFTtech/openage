// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#include "target_in_range.h"

#include <nyan/nyan.h>

#include "log/log.h"

#include "gamestate/api/ability.h"
#include "gamestate/component/api_component.h"
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
	log::log(DBG << "Checking TargetInRange cvondition for entity " << entity->get_id());

	auto command_queue = std::dynamic_pointer_cast<gamestate::component::CommandQueue>(
		entity->get_component(component::component_t::COMMANDQUEUE));
	auto target = command_queue->get_target().get(time);

	if (not target.has_value()) {
		// No target exists, exit early
		log::log(DBG << "Target for entity " << entity->get_id() << " is not set");
		return false;
	}

	// Get the component type matching the ability in the condition
	auto checked_ability_obj = condition->get<nyan::Object>("TargetInRange.ability");
	auto component_type = api::APIAbility::get_component_type(*checked_ability_obj);
	if (not entity->has_component(component_type)) {
		// Entity does not have the component matching the ability, exit early
		log::log(DBG << "Entity " << entity->get_id() << " does not have a component matching ability '"
		             << checked_ability_obj->get_name() << "'");
		return false;
	}

	// Get the actual ability used for the range check
	// this step is necessary because the ability defined by the condition
	// may be abstract, so multiple abilities may be valid
	auto component = std::dynamic_pointer_cast<gamestate::component::APIComponent>(
		entity->get_component(component_type));
	auto used_abilty_obj = component->get_ability();

	// Fetch min/max range from the ability
	nyan::value_float_t min_range = 0;
	nyan::value_float_t max_range = 0;
	if (api::APIAbility::check_property(used_abilty_obj, api::ability_property_t::RANGED)) {
		// Get min/max range from the property of the ability
		log::log(DBG << "Ability " << used_abilty_obj.get_name() << " has Ranged property");

		auto range_obj = api::APIAbility::get_property(used_abilty_obj, api::ability_property_t::RANGED);
		min_range = range_obj.get_float("Ranged.min_range");
		max_range = range_obj.get_float("Ranged.max_range");
	}
	log::log(DBG << "Min/Max range for ability " << used_abilty_obj.get_name() << ": "
	             << min_range << "/" << max_range);

	// Fetch the current position of the entity
	auto position = std::dynamic_pointer_cast<gamestate::component::Position>(
		entity->get_component(component::component_t::POSITION));
	auto current_pos = position->get_positions().get(time);

	if (std::holds_alternative<coord::phys3>(target.value())) {
		// Target is a position
		log::log(DBG << "Target is a position");

		auto target_pos = std::get<coord::phys3>(target.value());
		log::log(DBG << "Target position: " << target_pos);

		auto distance = (target_pos - current_pos).length();
		log::log(DBG << "Distance to target position: " << distance);

		return (distance >= min_range and distance <= max_range);
	}

	// Target is a game entity
	auto target_entity_id = std::get<entity_id_t>(target.value());
	log::log(DBG << "Target is a game entity with ID " << target_entity_id);
	if (not state->has_game_entity(target_entity_id)) {
		// Target entity does not exist
		log::log(DBG << "Target entity " << target_entity_id << " does not exist in state");
		return false;
	}

	auto target_entity = state->get_game_entity(target_entity_id);
	auto target_position = std::dynamic_pointer_cast<gamestate::component::Position>(
		target_entity->get_component(component::component_t::POSITION));
	auto target_pos = target_position->get_positions().get(time);
	log::log(DBG << "Target entity " << target_entity_id << " position: " << target_pos);

	auto distance = (target_pos - current_pos).length();
	log::log(DBG << "Distance to target entity " << target_entity_id << ": " << distance);

	return (distance >= min_range and distance <= max_range);
}

} // namespace openage::gamestate::activity
