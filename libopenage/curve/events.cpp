// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "events.h"

#include <ncurses.h>

namespace openage {
namespace curve {

#define GUI

// FIXME convert all these std::functions into our own datatype that stores all requirements.


EventQueue::Handle EventQueue::addcallback (const curve_time_t &at,
                                            const EventQueue::Eventclass &eventclass,
                                            const EventQueue::callback &trigger) {
	return addcallback(Event(at, eventclass, trigger, ++last_event_id));
}


EventQueue::Handle EventQueue::addcallback (const curve_time_t &at,
                                            const EventQueue::Eventclass &eventclass,
                                            const EventQueue::callback_void &trigger) {
	return addcallback(at, eventclass,
	                   [trigger](EventQueue *q, const curve_time_t &) {
		                   trigger(q);
	                   });
}


EventQueue::Handle EventQueue::addcallback (const EventQueue::Event &e) {

	auto it = queue.begin();
// Iterate to find the right insertion position
	for (; it != queue.end() && it->time < e.time; ++it) {}

	queue.insert(it, e);

	// todo clear the past
	return Handle(e.event_id);
}


void EventQueue::reschedule(const EventQueue::Handle &handle,
                            const curve_time_t &new_time) {
	auto it = resolve_handle(handle);
	if (it != queue.end()) {
		Event evnt = *it;
		queue.erase(it);
		evnt.time = new_time;
		addcallback(evnt);
	} else {
		// the event was either not valid or has already been executed.
	}
}


void EventQueue::execute_until(const curve_time_t &time) {
#ifndef GUI
	std::cout << "\rNOW: " << time;
#endif

	auto it = queue.begin();
	int row = 1;
	while ((it = queue.begin()) != queue.end() && it->time < time) {
		auto tmp = *it;
		queue.erase(it);
#ifndef GUI
		std::cout << " Exec " << tmp.event_id << " at " << tmp.time << "  " << std::endl;
#else
		mvprintw(row, 75, "X");
#endif

		it->event(this, tmp.time);
		past.push_back(tmp);

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
		std::cout << i.time << ": " << i.event_id << i.event_eventclass <<std::endl;
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

/*
void EventQueue::trigger(const curve_time_t &from, const curve_time_t &to) {
	int counter = 0;
	for (auto it = this->queue.between(from, to);
	    !cleared && it != queue.end();
	    ) {
		counter ++;

		auto time = it.value().time;
		if (it.value().precond(time)) {
			it.value().event(this, time);
		}
		//this->queue.erase(it);
		//if (time >= to) break;
		++it;
		//it = this->queue.between(time, to);
	}
	if (this->cleared)
		this->queue.clear();
	this->cleared = false;
	if (counter) counter = 0;
}
*/

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


}} // eventclassspace openage::curve
