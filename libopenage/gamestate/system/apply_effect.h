// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "time/time.h"


namespace openage {

namespace gamestate {
class GameEntity;
class GameState;

namespace system {


class ApplyEffect {
	/**
	 * Apply the effect of an ability to a game entity.
	 *
	 * @param entity Game entity applying the effects.
	 * @param state Game state.
	 * @param target Target entity of the effects.
	 * @param start_time Start time of change.
	 *
	 * @return Runtime of the change in simulation time.
	 */
	static const time::time_t apply_effect(const std::shared_ptr<gamestate::GameEntity> &entity,
	                                       const std::shared_ptr<openage::gamestate::GameState> &state,
	                                       const std::shared_ptr<gamestate::GameEntity> &target,
	                                       const time::time_t &start_time);
};

} // namespace system
} // namespace gamestate
} // namespace openage
