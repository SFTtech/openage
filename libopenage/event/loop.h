// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <list>
#include <memory>
#include <unordered_map>

#include "../curve/curve.h"
#include "eventqueue.h"
#include "event.h"
#include "../log/log.h"

namespace openage::event {

// The demo wants to display internal details
namespace demo {
int curvepong();
}

class Event;
class EventTarget;
class State;



/**
 * The core class to manage event class and targets.
 */
class Loop {

	// because the demo function displays internal info.
	friend int demo::curvepong();

public:
	/** register a new event class */
	void add_event_class(const std::shared_ptr<EventClass> &cls);

	/**
	 * Add a new Event to the queue.
	 * The event is from the given class (name) and is invoked for a
	 * target in a state.
	 *
	 * The `reference_time` is used to calculate the actual event time.
	 */
	std::weak_ptr<Event> create_event(const std::string &name,
	                                  const std::shared_ptr<EventTarget> &target,
	                                  const std::shared_ptr<State> &state,
	                                  const curve::time_t &reference_time,
	                                  const EventClass::param_map &params=EventClass::param_map({}));

	/**
	 * This will generate a new randomly named eventclass for this specific element
	 *
	 * The `reference_time` is used to determine the actual event trigger time.
	 */
	std::weak_ptr<Event> create_event(const std::shared_ptr<EventClass> &eventclass,
	                                  const std::shared_ptr<EventTarget> &target,
	                                  const std::shared_ptr<State> &state,
	                                  const curve::time_t &reference_time,
	                                  const EventClass::param_map &params=EventClass::param_map({}));

	/**
	 * Execute all events that are registered until a certain point in time.
	 */
	void reach_time(const curve::time_t &max_time,
	                   const std::shared_ptr<State> &state);

	/**
	 * Register that a given event must be reevaluated at a time,
	 * this usually happens because this event depended on an eventtarget
	 * that got changed at this time.
	 * This inserts the event into the changes queue
	 * so it will be evaluated in the next loop iteration.
	 */
	void create_change(const std::shared_ptr<Event> &event,
	                   const curve::time_t &changes_at);

	const EventQueue &get_queue() const {
		return this->queue;
	}

private:
	/**
	 * Execute the events
	 *
	 * @returns number of events processed
	 */
	int execute_events(const curve::time_t &max_time,
	                   const std::shared_ptr<State> &state);

	/**
	 * Call all the time change functions. This is constant on the state!
	 */
	void update_changes(const std::shared_ptr<State> &state);

	/**
	 * Here we do the bookkeeping of registered event classes.
	 */
	std::unordered_map<std::string, std::shared_ptr<EventClass>> classstore;

	/**
	 * All events are enqueued here.
	 */
	EventQueue queue;

	/**
	 * The currently processed event.
	 * This is useful for event cancelations (so one can't cancel itself).
	 */
	std::shared_ptr<Event> active_event;
};

} // openage::event
