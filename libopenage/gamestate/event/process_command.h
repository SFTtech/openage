// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "event/eventhandler.h"
#include "time/time.h"


namespace openage {

namespace event {
class EventLoop;
class Event;
class EventEntity;
class State;
} // namespace event


namespace gamestate {
class GameEntity;
class GameState;

namespace event {
/**
 * Process a command from a game entity command queue.
 */
class ProcessCommandHandler : public openage::event::OnceEventHandler {
public:
	ProcessCommandHandler();
	~ProcessCommandHandler() = default;

	void setup_event(const std::shared_ptr<openage::event::Event> &event,
	                 const std::shared_ptr<openage::event::State> &state) override;

	void invoke(openage::event::EventLoop &loop,
	            const std::shared_ptr<openage::event::EventEntity> &target,
	            const std::shared_ptr<openage::event::State> &state,
	            const time::time_t &time,
	            const param_map &params) override;

	time::time_t predict_invoke_time(const std::shared_ptr<openage::event::EventEntity> &target,
	                                 const std::shared_ptr<openage::event::State> &state,
	                                 const time::time_t &at) override;
};


/**
 * Primer for process command events in the activity system.
 *
 * @param time Current simulation time.
 * @param entity Game entity.
 * @param loop Event loop that the event is registered on.
 * @param state Game state.
 * @param next_id ID of the next node in the activity graph.
 *
 * @return Scheduled event.
 */
std::shared_ptr<openage::event::Event> primer_process_command(const time::time_t &,
                                                              const std::shared_ptr<gamestate::GameEntity> &entity,
                                                              const std::shared_ptr<openage::event::EventLoop> &loop,
                                                              const std::shared_ptr<gamestate::GameState> &state,
                                                              size_t next_id);

} // namespace event
} // namespace gamestate
} // namespace openage
