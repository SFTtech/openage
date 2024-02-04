// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "event/eventhandler.h"
#include "event/eventqueue.h"
#include "time/time.h"


namespace openage::event {

// The demo wants to display internal details
namespace demo {
int curvepong();
}

class Event;
class EventEntity;
class State;


/**
 * The core class to manage event handler and targets.
 */
class EventLoop {
	// because the demo function displays internal info.
	friend int demo::curvepong();

public:
	/**
	 * Create a new event loop.
	 */
	EventLoop() = default;
	~EventLoop() = default;

	/**
	 * Register a new event handler.
	 *
	 * Created event can reference the event handler ID to invoke it on
	 * execution.
	 *
	 * @param eventhandler Event handler.
	 */
	void add_event_handler(const std::shared_ptr<EventHandler> eventhandler);

	/**
	 * Add a new event to the queue using a registered event handler.
	 *
	 * @param eventhandler Event handler ID. The handler must already be registered on the loop.
	 * @param target Target entity. Can be \p nullptr.
	 * @param state Global state.
	 * @param reference_time Reference time to calculate the event execution time. The actual
	 *                       depends execution time on the type of event and may be changed
	 *                       by other events.
	 * @param params Event parameters map (default = {}). Passed to the event handler on event execution.
	 */
	std::shared_ptr<Event> create_event(const std::string eventhandler,
	                                    const std::shared_ptr<EventEntity> target,
	                                    const std::shared_ptr<State> state,
	                                    const time::time_t reference_time,
	                                    const EventHandler::param_map params = EventHandler::param_map({}));

	/**
	 * Add a new event to the queue using an arbritary event handler. If an event handler
	 * with the same ID is already registered, the registered event handler will be used
	 * instead.
	 *
	 * TODO: Why use this function when one can simply add the event handler and use the other
	 *      create_event function?
	 *
	 * @param eventhandler Event handler.
	 * @param target Target entity. Can be \p nullptr.
	 * @param state Global state.
	 * @param reference_time Reference time to calculate the event execution time. The actual
	 *                       depends execution time on the type of event and may be changed
	 *                       by other events.
	 * @param params Event parameters map (default = {}). Passed to the event handler on event execution.
	 */
	std::shared_ptr<Event> create_event(const std::shared_ptr<EventHandler> eventhandler,
	                                    const std::shared_ptr<EventEntity> target,
	                                    const std::shared_ptr<State> state,
	                                    const time::time_t reference_time,
	                                    const EventHandler::param_map params = EventHandler::param_map({}));

	/**
	 * Execute events in the queue with execution time <= a given point in time.
	 *
	 * @param time_until Maximum time until which events are executed.
	 * @param state Global state.
	 */
	void reach_time(const time::time_t &time_until,
	                const std::shared_ptr<State> &state);

	/**
	 * Initiate a reevaluation of a given event at a given time.
	 *
	 * This usually happens because this event depended on an event entity
	 * that got changed at this time.
	 *
	 * This inserts the event into the changes queue
	 * so it will be evaluated in the next loop iteration.
	 *
	 * @param event Event to reevaluate.
	 * @param changes_at Time at which the event should be reevaluated.
	 */
	void create_change(const std::shared_ptr<Event> event,
	                   const time::time_t changes_at);

	/**
	 * Get the event queue.
	 *
	 * @return Event queue.
	 */
	const EventQueue &get_queue() const {
		return this->queue;
	}

private:
	/**
	 *  Execute events in the queue with execution time <= a given point in time.
	 *
	 * @param time_until Maximum time until which events are executed.
	 * @param state Global state.
	 *
	 * @returns number of events processed
	 */
	int execute_events(const time::time_t &time_until,
	                   const std::shared_ptr<State> &state);

	/**
	 * Call all the time change functions. This is constant on the state!
	 *
	 * @param state Global state.
	 */
	void update_changes(const std::shared_ptr<State> &state);

	/**
	 * Here we do the bookkeeping of registered event handleres.
	 */
	std::unordered_map<std::string, std::shared_ptr<EventHandler>> classstore;

	/**
	 * All events are enqueued here.
	 */
	EventQueue queue;

	/**
	 * The currently processed event.
	 * This is useful for event cancelations (so one can't cancel itself).
	 */
	std::shared_ptr<Event> active_event;

	/**
	 * Mutex for protecting threaded access.
	 */
	std::recursive_mutex mutex;
};

} // namespace openage::event
