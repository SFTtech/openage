// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "events.h"
#include "trigger.h"

#include <ncurses.h>

namespace openage {
namespace curve {

// FIXME convert all these std::functions to a custom datatype to remove multiple level of stack indirection

EventQueue::Handle EventQueue::addcallback (
	const Eventclass &eventclass,
	const curve_time_t &initial_now,
	const time_predictor &at,
	const callback &event,
	const std::vector<Trigger *> dependents)
{
	auto it = queue.begin();
	for (; it != queue.end() && it->eventclass != eventclass; ++it) {}

	Event e;
	if (it != queue.end()) {
		e = *it;
		e.time = at(initial_now);
		e.event = event;
		queue.erase(it);
	} else {
		e = Event(at, initial_now, eventclass, event, ++last_event_id, dependents);

		// FIXME Register all on change events
	}

	return this->addcallback(e);
}


EventQueue::Handle EventQueue::schedule (
	const Eventclass &eventclass,
	const curve_time_t &initial_now,
	const time_predictor &at,
	const callback &event,
	const std::vector<Trigger *> dependents)
{
	auto it = queue.begin();
	for (; it != queue.end() && it->eventclass != eventclass; ++it) {}

	Event e;
	if (it != queue.end()) {
		e = *it;
		e.time = at(initial_now);
		e.event = event;
		queue.erase(it);
	} else {
		e = Event(at, initial_now, eventclass, event, ++last_event_id, dependents, true);
		// FIXME Register all on change events
	}

	auto handle = this->addcallback(e);

	for (const auto &t : dependents) {
		t->on_change_future(
			std::string("dependency_") + eventclass,
			[handle](EventQueue *q, const curve_time_t &t) {
				q->reschedule(handle, t);
			});
	}
	return handle;


}


EventQueue::Handle EventQueue::addcallback (
	const Eventclass &eventclass,
	const curve_time_t &initial_now,
	const time_predictor &at,
	const callback_void &event,
	const std::vector<Trigger *> dependents)
{
	return addcallback(
		eventclass,
		initial_now,
		at,
		[event](EventQueue *q, const curve_time_t &) {
			event(q);
		}, dependents);
}


EventQueue::Handle EventQueue::addcallback (const Event &e) {
	auto it = queue.begin();
	for (; it != queue.end() && it->time < e.time; ++it) {}
	queue.insert(it, e);

	return Handle(e.event_id);
}


void EventQueue::reschedule(const EventQueue::Handle &handle,
                            const EventQueue::time_predictor &new_timer,
                            const curve_time_t &at) {
	auto it = resolve_handle(handle);
	if (it != queue.end()) {
		Event evnt = *it;
		queue.erase(it);
		evnt.predictor = new_timer;
		evnt.time = new_timer(at);
		addcallback(evnt);
	} else {
		// the event was either not valid or has already been executed.
	}
}


void EventQueue::reschedule(const EventQueue::Handle &handle, const curve_time_t &at) {
	auto it = resolve_handle(handle);
	if (it != queue.end()) {
		Event evnt = *it;
		queue.erase(it);
		evnt.time = evnt.predictor(at);
		addcallback(evnt);
	} else {
		// the event was either not valid or has already been executed.
	}
}


void EventQueue::reschedule(const EventQueue::time_predictor &new_timer, const curve_time_t &at) {
	if (active_event != nullptr) {
		auto evnt = *active_event;
		evnt.predictor = new_timer;
		evnt.time = evnt.predictor(at);
		addcallback(evnt);
	}
}


void EventQueue::reschedule(const curve_time_t &at) {
	if (active_event != nullptr) {
		auto evnt = *active_event;
		evnt.time = evnt.predictor(at);
		addcallback(evnt);
	}
}


void EventQueue::execute_until(const curve_time_t &time) {
	auto it = queue.begin();
	while ((it = queue.begin()) != queue.end() && it->time < time) {
		auto tmp = *it;
		queue.erase(it);

		this->active_event = &tmp;
		it->event(this, tmp.time);
		active_event = nullptr;

		if (tmp.repeat) {
			// if the event is to be scheduled again after execution
			// just insert it again
			tmp.time = tmp.predictor(tmp.time);
			addcallback(tmp);
		} else {
			past.push_back(tmp);
		}

		if (this->cleared) {
			this->cleared = false;
			break;
		}
	}
}


void EventQueue::remove(const EventQueue::Handle &handle) {
	for (auto it = queue.begin(); it != queue.end(); ++it) {
		if (it->event_id == handle.event_id) {
			it = queue.erase(it);
			break;
		}
	}
}


std::deque<EventQueue::Event>::iterator EventQueue::resolve_handle(
	const EventQueue::Handle &handle) {
	for (auto it = queue.begin(); it != queue.end(); ++it) {
		if (it->event_id == handle.event_id) {
			return it;
		}
	}
	return queue.end();
}


void EventQueue::print() {
#ifndef GUI
	std::cout << "Queue: " << std::endl;
	for (const auto &i : queue) {
		std::cout << i.time << ": " << i.event_id << i.eventclass <<std::endl;
	}

	std::cout << std::endl;

#else
	mvprintw(0, 100, "Queue: Size %i ", queue.size());
	int row = 1;
	for (const auto &i : queue) {
		mvprintw(row, 100, "T: %f [%i] %s", i.time, i.event_id, i.eventclass.c_str());
		row ++;
	}
#endif
}


void EventQueue::clear() {
#ifndef GUI
	std::cout << "CLEAR" << std::endl;
#endif
	// set cleared before actually clearing to avoid races
	this->cleared = true;
	this->queue.clear();
}


void EventQueue::clear(const curve_time_t &time) {
#ifndef GUI
	std::cout << "CLEAR " << time << std::endl;
#endif
	auto it = queue.begin();
	for (; it != queue.end() && it->time < time; ++it) {}
	for (; it != queue.end(); it = queue.erase(it) ) {}
}


}} // namespace openage::curve
