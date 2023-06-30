// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "curve/curve.h"
#include "gamestate/system/types.h"


namespace openage {

namespace event {
class EventLoop;
}

namespace gamestate {
class GameEntity;
class GameState;

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
	                    const curve::time_t &start_time,
	                    const std::shared_ptr<openage::event::EventLoop> &loop,
	                    const std::shared_ptr<openage::gamestate::GameState> &state);

private:
	/**
	 * Run a built-in engine subsystem.
	 *
	 * @param entity Game entity.
	 * @param start_time Start time of change.
	 * @param system_id ID of the subsystem to run.
	 *
     * @return Runtime of the change in simulation time.
	 */
	static const curve::time_t handle_subsystem(const std::shared_ptr<gamestate::GameEntity> &entity,
	                                            const curve::time_t &start_time,
	                                            system_id_t system_id);
};

} // namespace system
} // namespace gamestate
} // namespace openage
