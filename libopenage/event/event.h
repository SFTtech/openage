// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "eventclass.h"
#include "../curve/curve.h"

namespace openage::event {

class EventQueue;
class EventTarget;

/**
 * The actual one event that may be called - it is used to manage the event itself.
 * It does not need to be stored.
 */
class Event : public std::enable_shared_from_this<Event> {
public:
	Event(const std::shared_ptr<EventTarget> &trgt,
	      const std::shared_ptr<EventClass> &eventclass,
	      EventClass::param_map params);

	const std::weak_ptr<EventTarget> &get_target() const {
		return this->target;
	}

	const std::shared_ptr<EventClass> &get_eventclass() const {
		return this->eventclass;
	}

	/**
	 * Reschedule will call the predict_invoke_time method to initiate a reschedule
	 * for the event it uses the reference_time as base for its calculation
	 */
	void reschedule(const curve::time_t reference_time);

	size_t hash() const {
		return this->myhash;
	}

	const curve::time_t &get_time() const {
		return this->time;
	}

	void set_time(const curve::time_t &t) {
		this->time = t;
	}

	const EventClass::param_map &get_params() const {
		return this->params;
	}

	/**
	 * Let this event depend on another an event target.
	 * When this target is changes, the event is reevaluated.
	 *
	 * To be called in the EventClass::setup function.
	 */
	void depend_on(const std::shared_ptr<EventTarget> &dependency);

	/**
	 * For sorting events by their trigger time.
	 */
	bool operator <(const Event &other) const;

	curve::time_t last_triggered = 0;

private:
	/**
	 * Parameters for the event (determined by its EventClass)
	 */
	EventClass::param_map params;

	/** The actor that has the event */
	std::weak_ptr<EventTarget> target;

	/** Type of this event. */
	std::shared_ptr<EventClass> eventclass;

	/** Time this event occurs/occured */
	curve::time_t time;

	/** Precalculated std::hash for the event */
	size_t myhash;
};


} // openage::event
