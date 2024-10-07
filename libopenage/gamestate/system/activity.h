// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <optional>

#include "event/eventhandler.h"
#include "gamestate/system/types.h"
#include "time/time.h"


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
	 * @param start_time Start time of change.
	 * @param entity Game entity.
	 */
	static void advance(const time::time_t &start_time,
	                    const std::shared_ptr<gamestate::GameEntity> &entity,
	                    const std::shared_ptr<openage::event::EventLoop> &loop,
	                    const std::shared_ptr<openage::gamestate::GameState> &state,
	                    const std::optional<openage::event::EventHandler::param_map> &ev_params = std::nullopt);

private:
	/**
	 * Run a built-in engine subsystem.
	 *
	 * @param start_time Start time of change.
	 * @param entity Game entity.
	 * @param state Game state.
	 * @param system_id ID of the subsystem to run.
	 *
	 * @return Runtime of the change in simulation time.
	 */
	static const time::time_t handle_subsystem(const time::time_t &start_time,
	                                           const std::shared_ptr<gamestate::GameEntity> &entity,
	                                           const std::shared_ptr<openage::gamestate::GameState> &state,
	                                           system_id_t system_id);
};

} // namespace system
} // namespace gamestate
} // namespace openage
