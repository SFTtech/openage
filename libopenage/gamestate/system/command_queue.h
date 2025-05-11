// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "time/time.h"


namespace openage::gamestate {
class GameEntity;

namespace system {

class CommandQueue {
public:
    /**
     * Clear the command queue of the entity.
     *
     * @param entity Game entity.
     * @param start_time Start time of change.
     *
     * @return Runtime of the change in simulation time.
     */
    static const time::time_t clear_queue(const std::shared_ptr<gamestate::GameEntity> &entity,
                                          const time::time_t &start_time);

	/**
	 * Pop the front command from the command queue of the entity.
	 *
	 * @param entity Game entity.
	 * @param start_time Start time of change.
	 *
	 * @return Runtime of the change in simulation time.
	 */
	static const time::time_t pop_command(const std::shared_ptr<gamestate::GameEntity> &entity,
	                                      const time::time_t &start_time);
};

} // namespace system
} // namespace openage::gamestate
