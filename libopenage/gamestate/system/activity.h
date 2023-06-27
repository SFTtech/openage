// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "curve/curve.h"


namespace openage::gamestate {
class GameEntity;

namespace system {

class Activity {
public:
	/**
     * Advance in the activity flow graph of the game entity.
     *
     * @param entity Game entity.
     * @param start_time Start time of change.
     */
	static void advance(const std::shared_ptr<gamestate::GameEntity> &entity,
	                    const curve::time_t &start_time);
};

} // namespace system
} // namespace openage::gamestate
