// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

// for eventclass::types
#include "eventclass.h"

#include "../curve.h"

#include "../../util/hash.h"

#include <deque>
#include <memory>
#include <unordered_set>

namespace openage {
class State;
namespace curve {

class Event;
class EventManager;
class EventTarget;

/**
 * The core event class for execution and execution dependencies.
 */
class EventQueue {
	friend class EventManager;
public:
	EventQueue();

	/**
	 * Add an event for a specified target
	 *
	 * A target is every single unit in the game word - so best add these events
	 * in the constructor of the game objects.
	 */
	std::weak_ptr<Event> add(const std::shared_ptr<EventTarget> &eventtarget,
	                         const std::shared_ptr<EventClass> &eventclass,
	                         const std::shared_ptr<State> &state,
	                         const curve_time_t &reference_time,
                             const EventClass::param_map &params);

	void remove(const std::shared_ptr<Event> &evnt);

	/**
	 * The event may be already part of the execution queue,
	 * so update it, if it shall be executed in the future.
	 */
	void update(const std::shared_ptr<Event> &);

	/**
	 * The event was just removed.
	 */
	void readd(const std::shared_ptr<Event> &);

	/**
	 * An event target has changed, and the event shall be retriggered
	 */
	void add_change(const std::shared_ptr<Event> &event, const curve_time_t &changed_at);

	/**
	 * Get an event identified by EventTarget and EventClass, or a shared_ptr<nullptr> if not found
	 */
	const std::shared_ptr<Event> &get_evnt(const std::shared_ptr<EventTarget> &,
	                                       const std::shared_ptr<EventClass> &);

	/**
	 * get an accessor to the running queue for state ouput purpose
	 */
	const std::deque<std::shared_ptr<Event>> &ro_queue() const { return queue; }

private:
	void check_in(const std::shared_ptr<Event> &evnt);

	class OnChangeElement {
	public:
		OnChangeElement(const std::shared_ptr<Event> &evnt,
		                const curve_time_t &time);

		curve_time_t time;
		std::weak_ptr<Event> evnt;
		const size_t hash;

		class Hasher : public std::unary_function<OnChangeElement, size_t> {
		public:
			size_t operator()(const OnChangeElement& e) const {
				return e.hash;
			}
		};

		class Equal : public std::unary_function<OnChangeElement, size_t> {
		public:
			size_t operator()(const OnChangeElement& left,
			                  const OnChangeElement& right) const;
		};
	};

	using change_set = std::unordered_set<OnChangeElement,
	                                      OnChangeElement::Hasher,
	                                      OnChangeElement::Equal>;
	// Implement double buffering around changesets, that we do not run into deadlocks
	change_set *changes;
	change_set *future_changes;

	change_set changeset_A;
	change_set changeset_B;

	std::deque<std::shared_ptr<Event>> &select_queue(EventClass::Type type);

	std::deque<std::shared_ptr<Event>> on_change; // will trigger itself and add to the queue
	std::deque<std::shared_ptr<Event>> on_change_immediately; // will trigger itself and at to the queue
	std::deque<std::shared_ptr<Event>> on_keyframe; // will trigger itself
	// TODO is this required?
	std::deque<std::shared_ptr<Event>> on_execute; // whenever one of these is executed, it shall be reinserted
	// Type::ONCE is only inserted into the queue


	std::deque<std::shared_ptr<Event>> queue;


	std::shared_ptr<Event> null_evnt;
};

}} // namespace openage::curve
