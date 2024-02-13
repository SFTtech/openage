// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include "event/evententity.h"
#include "event/eventhandler.h"
#include "time/time.h"


namespace openage {

namespace event {
class EventLoop;
class Event;
class State;
} // namespace event

namespace gamestate {

class EntityFactory;

namespace event {

// TODO: This is only for testing
class Spawner : public openage::event::EventEntity {
public:
	Spawner(const std::shared_ptr<openage::event::EventLoop> &loop);
	~Spawner() = default;

	size_t id() const override;
	std::string idstr() const override;
};

/**
 * Handle the creation of entities in the game world
 */
class SpawnEntityHandler : public openage::event::OnceEventHandler {
public:
	/**
	 * Creates a new SpawnEntityHandler.
	 *
	 * @param loop: Event loop that the components register on.
	 * @param factory: Factory that is used to create the entity.
	 */
	SpawnEntityHandler(const std::shared_ptr<openage::event::EventLoop> &loop,
	                   const std::shared_ptr<gamestate::EntityFactory> &factory);
	~SpawnEntityHandler() = default;

	/**
	 * Called for each event that is created for this EventHandler.
	 * The job of the setup function is to add all dependencies with other event
	 * targets found in state.
	 */
	void setup_event(const std::shared_ptr<openage::event::Event> &event,
	                 const std::shared_ptr<openage::event::State> &state) override;

	/**
	 * This method implements the effects of the event.
	 * It will be called at the time that was determined by `predict_invoke_time`.
	 *
	 * Called from the Loop.
	 */
	void invoke(openage::event::EventLoop &loop,
	            const std::shared_ptr<openage::event::EventEntity> &target,
	            const std::shared_ptr<openage::event::State> &state,
	            const time::time_t &time,
	            const param_map &params) override;

	/**
	 * Is called to calculate the execution time for an event of this eventhandler.
	 * This is called whenever one of the set up dependencies was changed,
	 * or when a REPEAT event was executed.
	 *
	 * @param target: the target the event was created for
	 * @param state: the state this shall work on
	 * @param at: the time when the change happened, from there on it shall be
	 *            calculated onwards
	 *
	 * If the event is obsolete, return <time_t>::min().
	 *
	 * If the time is lower than the previous time,
	 * then dependencies may not be resolved perfectly anymore
	 * (if other events have already been calculated before that).
	 */
	time::time_t predict_invoke_time(const std::shared_ptr<openage::event::EventEntity> &target,
	                                 const std::shared_ptr<openage::event::State> &state,
	                                 const time::time_t &at) override;

private:
	/**
	 * Event loop that the entity components are registered on.
	 */
	std::shared_ptr<openage::event::EventLoop> loop;

	/**
	 * The factory that is used to create the entity.
	 */
	std::shared_ptr<gamestate::EntityFactory> factory;
};

} // namespace event
} // namespace gamestate
} // namespace openage
