// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#include "move.h"

#include <compare>
#include <vector>

#include <nyan/nyan.h>

#include "log/log.h"
#include "log/message.h"

#include "coord/phys.h"
#include "curve/continuous.h"
#include "curve/segmented.h"
#include "gamestate/api/ability.h"
#include "gamestate/api/animation.h"
#include "gamestate/api/property.h"
#include "gamestate/api/types.h"
#include "gamestate/component/api/move.h"
#include "gamestate/component/api/turn.h"
#include "gamestate/component/internal/command_queue.h"
#include "gamestate/component/internal/commands/move.h"
#include "gamestate/component/internal/position.h"
#include "gamestate/component/types.h"
#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"
#include "gamestate/map.h"
#include "pathfinding/path.h"
#include "pathfinding/pathfinder.h"
#include "util/fixed_point.h"


namespace openage::gamestate::system {


std::vector<coord::phys3> find_path(const std::shared_ptr<path::Pathfinder> &pathfinder,
                                    path::grid_id_t grid_id,
                                    const coord::phys3 &start,
                                    const coord::phys3 &end,
                                    const time::time_t &start_time) {
	auto start_tile = start.to_tile();
	auto end_tile = end.to_tile();

	// Search for a path between the start and end tiles
	path::PathRequest request{
		grid_id,
		start_tile,
		end_tile,
		start_time,
	};
	auto tile_path = pathfinder->get_path(request);

	if (tile_path.status != path::PathResult::FOUND) {
		// No path found
		return {};
	}

	// Get the waypoints of the path
	std::vector<coord::phys3> path;
	path.reserve(tile_path.waypoints.size());

	// Start poition is first waypoint
	path.push_back(start);

	// Pathfinder waypoints contain start and end tile; we can ignore them
	for (size_t i = 1; i < tile_path.waypoints.size() - 1; ++i) {
		auto tile = tile_path.waypoints.at(i);
		path.push_back(tile.to_phys3_center());
	}

	// End position is last waypoint
	path.push_back(end);

	return path;
}

const time::time_t Move::move_command(const std::shared_ptr<gamestate::GameEntity> &entity,
                                      const std::shared_ptr<openage::gamestate::GameState> &state,
                                      const time::time_t &start_time) {
	auto command_queue = std::dynamic_pointer_cast<component::CommandQueue>(
		entity->get_component(component::component_t::COMMANDQUEUE));
	auto command = std::dynamic_pointer_cast<component::command::MoveCommand>(
		command_queue->pop_command(start_time));

	if (not command) [[unlikely]] {
		log::log(MSG(warn) << "Command is not a move command.");
		return time::time_t::from_int(0);
	}

	return Move::move_default(entity, state, command->get_target(), start_time);
}


const time::time_t Move::move_default(const std::shared_ptr<gamestate::GameEntity> &entity,
                                      const std::shared_ptr<openage::gamestate::GameState> &state,
                                      const coord::phys3 &destination,
                                      const time::time_t &start_time) {
	if (not entity->has_component(component::component_t::MOVE)) [[unlikely]] {
		log::log(WARN << "Entity " << entity->get_id() << " has no move component.");
		return time::time_t::from_int(0);
	}

	auto turn_component = std::dynamic_pointer_cast<component::Turn>(
		entity->get_component(component::component_t::TURN));
	auto turn_ability = turn_component->get_ability();
	auto turn_speed = turn_ability.get<nyan::Float>("Turn.turn_speed");

	auto move_component = std::dynamic_pointer_cast<component::Move>(
		entity->get_component(component::component_t::MOVE));
	auto move_ability = move_component->get_ability();
	auto move_speed = move_ability.get<nyan::Float>("Move.speed");
	auto move_path_grid = move_ability.get<nyan::ObjectValue>("Move.path_type");

	auto pos_component = std::dynamic_pointer_cast<component::Position>(
		entity->get_component(component::component_t::POSITION));

	auto &positions = pos_component->get_positions();
	auto &angles = pos_component->get_angles();
	auto current_pos = positions.get(start_time);
	auto current_angle = angles.get(start_time);

	// Find path
	auto map = state->get_map();
	auto pathfinder = map->get_pathfinder();
	auto grid_id = map->get_grid_id(move_path_grid->get_name());
	auto waypoints = find_path(pathfinder, grid_id, current_pos, destination, start_time);

	// use waypoints for movement
	double total_time = 0;
	pos_component->set_position(start_time, current_pos);
	for (size_t i = 1; i < waypoints.size(); ++i) {
		auto prev_waypoint = waypoints[i - 1];
		auto cur_waypoint = waypoints[i];

		auto path_vector = cur_waypoint - prev_waypoint;
		auto path_angle = path_vector.to_angle();

		// rotation
		if (not turn_speed->is_infinite_positive()) {
			auto angle_diff = path_angle - current_angle;
			if (angle_diff < 0) {
				// get the positive difference
				angle_diff = angle_diff * -1;
			}
			if (angle_diff > 180) {
				// always use the smaller angle
				angle_diff = angle_diff - 360;
				angle_diff = angle_diff * -1;
			}

			// Set an intermediate position keyframe to halt the game entity
			// until the rotation is done
			double turn_time = angle_diff.to_double() / turn_speed->get();
			total_time += turn_time;
			pos_component->set_position(start_time + total_time, prev_waypoint);

			// update current angle for next waypoint
			current_angle = path_angle;
		}
		pos_component->set_angle(start_time + total_time, path_angle);

		// movement
		double move_time = 0;
		if (not move_speed->is_infinite_positive()) {
			auto distance = path_vector.length();
			move_time = distance / move_speed->get();
		}
		total_time += move_time;

		pos_component->set_position(start_time + total_time, cur_waypoint);
	}

	// properties
	auto ability = move_component->get_ability();
	if (api::APIAbility::check_property(ability, api::ability_property_t::ANIMATED)) {
		auto property = api::APIAbility::get_property(ability, api::ability_property_t::ANIMATED);
		auto animations = api::APIAbilityProperty::get_animations(property);
		auto animation_paths = api::APIAnimation::get_animation_paths(animations);

		if (animation_paths.size() > 0) [[likely]] {
			entity->render_update(start_time, animation_paths[0]);
		}
	}

	return total_time;
}

} // namespace openage::gamestate::system
