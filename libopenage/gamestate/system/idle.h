// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "time/time.h"


namespace openage::gamestate {
class GameEntity;

namespace system {

class Idle {
public:
	/**
	 * Let a game entity idle.
	 *
	 * This does not change the state of a unit. It only changes its animation and
	 * sounds.
	 *
	 * @param entity Game entity.
	 * @param start_time Start time of change.
	 *
	 * @return Runtime of the change in simulation time.
	 */
	static const time::time_t idle(const std::shared_ptr<gamestate::GameEntity> &entity,
	                               const time::time_t &start_time);
};

} // namespace system
} // namespace openage::gamestate
