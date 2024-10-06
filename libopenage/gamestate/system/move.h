// Copyright 2023-2024 the openage authors. See copying.md for legal info.

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
	 * Move a game entity to a destination.
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
