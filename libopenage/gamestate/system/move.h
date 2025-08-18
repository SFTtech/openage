// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "coord/phys.h"
#include "time/time.h"


namespace openage::gamestate {
class GameEntity;
class GameState;

namespace system {

class Move {
public:
	/**
	 * Move a game entity to a destination from a move command.
	 *
	 * The front command in the command queue is expected to be of type `Move`. If
	 * not, the command is ignored and a runtime of 0 is returned.
	 *
	 * Consumes (pops) the move command from the command queue.
	 *
	 * @param entity Game entity.
	 * @param state Game state.
	 * @param start_time Start time of change.
	 *
	 * @return Runtime of the change in simulation time.
	 */
	static const time::time_t move_command(const std::shared_ptr<gamestate::GameEntity> &entity,
	                                       const std::shared_ptr<openage::gamestate::GameState> &state,
	                                       const time::time_t &start_time);

	/**
	 * Move a game entity to the current target of the game entity.
	 *
	 * The target is fetched from the command queue. If no target is set, the
	 * command is ignored and a runtime of 0 is returned.
	 *
	 * @param entity Game entity.
	 * @param state Game state.
	 * @param start_time Start time of change.
	 *
	 * @return Runtime of the change in simulation time.
	 */
	static const time::time_t move_target(const std::shared_ptr<gamestate::GameEntity> &entity,
	                                      const std::shared_ptr<openage::gamestate::GameState> &state,
	                                      const time::time_t &start_time);

private:
	/**
	 * Move a game entity to a destination.
	 *
	 * The entity requires an enabled `Move` and `Turn` component.
	 *
	 * The entity takes the following actions:
	 * - use the pathfinding system to find a waypoint path to the destination
	 * - for each waypoint:
	 *   - rotate towards the waypoint
	 *   - move to the waypoint
	 *
	 * These action mirror the movement behavior of units in Age of Empires II.
	 *
	 * @param entity Game entity.
	 * @param state Game state.
	 * @param destination Destination coordinates.
	 * @param start_time Start time of change.
	 *
	 * @return Runtime of the change in simulation time.
	 */
	static const time::time_t move_default(const std::shared_ptr<gamestate::GameEntity> &entity,
	                                       const std::shared_ptr<openage::gamestate::GameState> &state,
	                                       const coord::phys3 &destination,
	                                       const time::time_t &start_time);
};

} // namespace system
} // namespace openage::gamestate
