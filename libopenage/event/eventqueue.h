// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <unordered_set>

#include "eventclass.h"
#include "eventstore.h"
#include "../curve/curve.h"


namespace openage::event {

class Event;
class Loop;
class EventTarget;

/**
 * The core event class for execution and execution dependencies.
 */
class EventQueue final {
public:

	class OnChangeElement {
	public:
		OnChangeElement(const std::shared_ptr<Event> &evnt,
		                const curve::time_t &time);

		curve::time_t time;
		std::weak_ptr<Event> evnt;
		const size_t hash;

		class Hasher {
		public:
			size_t operator ()(const OnChangeElement& e) const {
				return e.hash;
			}
		};

		class Equal {
		public:
			size_t operator ()(const OnChangeElement& left,
			                   const OnChangeElement& right) const;
		};
	};

	/**
	 * Type for the set to store changes to track.
	 */
	using change_set = std::unordered_set<OnChangeElement,
	                                      OnChangeElement::Hasher,
	                                      OnChangeElement::Equal>;


	EventQueue();

	/**
	 * Add an event for a specified target
	 *
	 * A target is every single unit in the game world - so best add these events
	 * in the constructor of the game objects.
	 *
	 * The `insertion_time` is the time used to calculate when
	 * the actual event time will happen!
	 */
	std::shared_ptr<Event> create_event(const std::shared_ptr<EventTarget> &eventtarget,
	                                    const std::shared_ptr<EventClass> &eventclass,
	                                    const std::shared_ptr<State> &state,
	                                    const curve::time_t &reference_time,
	                                    const EventClass::param_map &params);

	/**
	 * Remove the given event from the queue.
	 */
	void remove(const std::shared_ptr<Event> &evnt);

	/**
	 * An update to existing events has to be applied.
	 * The execution time of this event may have changed or it
	 * is newly created. This updates/inserts the given event
	 * in the main queue.
	 */
	void enqueue_change(const std::shared_ptr<Event> &event);

	/**
	 * The event was just removed, add it again.
	 * This is used for REPEAT events so that they are repeated.
	 */
	void reenqueue(const std::shared_ptr<Event> &event);

	/**
	 * An event target has changed, and the event shall be retriggered
	 */
	void add_change(const std::shared_ptr<Event> &event,
	                const curve::time_t &changed_at);

	/**
	 * Get an accessor to the running queue for state output purpose.
	 */
	const EventStore &get_event_queue() const;

	/**
	 * Obtain the next event from the `event_queue` that happens before `<= max_time`.
	 */
	std::shared_ptr<Event> take_event(const curve::time_t &max_time);

	/**
	 * Get the change_set to process changes.
	 */
	const change_set &get_changes() const;

	/**
	 * All changes (fetched with `get_changes`) have been processed,
	 * so we can clear the change_set.
	 */
	void clear_changes();

	/**
	 * Swap the `changes` and `future_changes`.
	 */
	void swap_changesets();

private:

	// Implement double buffering around changesets, that we do not run into deadlocks
	// those point to the `changeset_A` and `changeset_B`.
	change_set *changes;
	change_set *future_changes;

	// storage for the double buffer in `changes` and `future_changes`.
	change_set changeset_A;
	change_set changeset_B;

	/**
	 * Stores events that sleep until their dependency changes.
	 */
	std::unordered_set<std::shared_ptr<Event>> dependency_events;

	/**
	 * Stores events that sleep until their dependency changes, but they trigger
	 * instantly when their dependency changes.
	 */
	std::unordered_set<std::shared_ptr<Event>> dependency_immediately_events;

	/**
	 * Events that fire only when triggered.
	 */
	std::unordered_set<std::shared_ptr<Event>> trigger_events;

	/**
	 * The universe timeline processes through this queue.
	 * Type::ONCE is only inserted into the queue.
	 */
	EventStore event_queue;
};

} // openage::event
