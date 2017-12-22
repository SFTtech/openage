// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "eventclass.h"

#include "../curve.h"

#include <memory>

namespace openage {
namespace curve {

class EventQueue;
class EventTarget;

/**
 * The actual one event that may be called - it is used to manage the event itself.
 * It does not need to be stored
 */
class Event {
	friend EventQueue;
    friend EventManager;
public:

	Event(const std::shared_ptr<EventTarget> &trgt,
	      const std::shared_ptr<EventClass> &eventclass,
	      const EventClass::param_map &params);

	std::weak_ptr<EventTarget> &target() { return _target; }
	std::shared_ptr<EventClass> &eventclass() {return _eventclass; }

	/**
	 * Reschedule will call the recalculate_time method to initiate a reschedule for the event
	 * it uses the reference_time as base for its calculation
	 */
	void reschedule(const curve_time_t reference_time);

	size_t hash() const { return myhash; }
	curve_time_t time() const { return _time; }
	void time(const curve_time_t &t) { _time = t; }

	curve_time_t last_triggered = 0;
private:
    EventClass::param_map params;

    std::weak_ptr<EventTarget> _target;
	std::shared_ptr<EventClass> _eventclass;
	size_t myhash;
	curve_time_t _time;
};


}} // namespace openage::curve
