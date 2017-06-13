// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "events.h"

namespace openage {
namespace curve {

void EventQueue::addcallback (const curve_time_t &at,
                              const EventQueue::condition &precond,
                              const EventQueue::callback &trigger) {
	Event e;
	e.event = trigger;
	e.precond = precond;
	e.time = at;

	addcallback(e);
}

void EventQueue::addcallback (const curve_time_t &at,
                              const EventQueue::condition_void &precond,
                              const EventQueue::callback_void &trigger) {
	Event e;
	e.event = [trigger](const curve_time_t &) { trigger(); };
	e.precond = [precond](const curve_time_t &) { return precond(); };
	e.time = at;

	addcallback(e);
}

void EventQueue::addcallback (const curve_time_t &at,
                              const EventQueue::callback &trigger) {
	this->addcallback(at, &EventQueue::_true, trigger);
}

void EventQueue::addcallback (const curve_time_t &at,
                              const EventQueue::callback_void &trigger) {
	this->addcallback(at, &EventQueue::_true, [trigger](const curve_time_t &) {
			trigger();
		});
}

void EventQueue::addcallback (const EventQueue::Event &e) {
	queue.insert(e.time, e);
}

void EventQueue::trigger(const curve_time_t &from, const curve_time_t &to) {
	for (auto it = queue.between(from, to); it != queue.end(); ++it) {
		if (it.value().precond(it.value().time)) {
			it.value().event(it.value().time);
		}
	}
}

bool EventQueue::_true(const curve_time_t &) {
	return true;
}

}} // namespace openage::curve
