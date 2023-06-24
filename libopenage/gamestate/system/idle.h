// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "curve/curve.h"


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
     */
	static void idle(const std::shared_ptr<gamestate::GameEntity> &entity,
	                 const curve::time_t &start_time);
};

} // namespace system
} // namespace openage::gamestate
