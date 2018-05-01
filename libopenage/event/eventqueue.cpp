// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#include "eventqueue.h"

#include <algorithm>

#include "event.h"
#include "eventclass.h"
#include "eventtarget.h"
#include "../log/log.h"


namespace openage::event {

std::weak_ptr<Event> EventQueue::create_event(const std::shared_ptr<EventTarget> &trgt,
                                              const std::shared_ptr<EventClass> &cls,
                                              const std::shared_ptr<State> &state,
                                              const curve::time_t &reference_time,
                                              const EventClass::param_map &params) {

	auto event = std::make_shared<Event>(trgt, cls, params);

	cls->setup_event(event, state);

	switch(cls->type) {
	case EventClass::trigger_type::DEPENDENCY:
	case EventClass::trigger_type::REPEAT:
	case EventClass::trigger_type::ONCE:
		event->set_time(event->get_eventclass()
		                     ->predict_invoke_time(trgt, state, reference_time));

		if (event->get_time() == std::numeric_limits<curve::time_t>::min()) {
			return std::weak_ptr<Event>();
		}
		break;

	case EventClass::trigger_type::DEPENDENCY_IMMEDIATELY:
	case EventClass::trigger_type::TRIGGER:
		event->set_time(reference_time);
		break;
	}

	log::log(DBG << "Queue: inserting event " << event->get_eventclass()->id() <<
	         " into queue to be executed at t=" << event->get_time());

	auto &queue = this->select_queue(event->get_eventclass()->type);
	queue.emplace_back(event);

	return event;
}


EventQueue::EventQueue()
	:
	changes(&changeset_A),
	future_changes(&changeset_B) {}


void EventQueue::add_change(const std::shared_ptr<Event> &event,
                            const curve::time_t &changed_at) {

	auto it = this->changes->find(OnChangeElement(event, changed_at));

	// Has the event been triggered in this round?
	if (event->last_triggered < changed_at) {
		// Is the change already in the queue?
		if (it != changes->end()) {
			// Is the new change dated _before_ the old one?
			if (it->time > changed_at) {
				log::log(DBG << "Queue: adjusting time in change queue");

				// Save the element
				OnChangeElement e = *it;
				e.time = changed_at;

				// delete it from the container and readd it
				it = this->changes->erase(it);
				it = this->changes->insert(it, e);
			}
			else {
				// this change is to be ignored
				log::log(DBG << "Queue: skipping change for " << event->get_eventclass()->id()
				         << " at " << changed_at
				         << " because there was already a better one at t=" << it->time);
			}
		}
		else {
			// the change was not in the to be changed list
			this->changes->emplace(event, changed_at);
			log::log(DBG << "Queue: inserting change for " << event->get_eventclass()->id()
			         << " to be applied at t=" << changed_at);
		}
	}
	else {
		// the event has been triggered in this round already, so skip it this time
		this->future_changes->emplace(event, changed_at);
		log::log(DBG << "Queue: putting change for "
		         << event->get_eventclass()->id() << " into the future, "
		         << "applied at t=" << changed_at);
	}

	event->last_triggered = changed_at;
}


void EventQueue::remove(const std::shared_ptr<Event> &evnt) {
	auto &queue = this->select_queue(evnt->get_eventclass()->type);
	auto it = std::find(std::begin(queue), std::end(queue), evnt);

	if (it != queue.end()) {
		queue.erase(it);
	}
}


std::deque<std::shared_ptr<Event>> &EventQueue::select_queue(EventClass::trigger_type type) {
	switch(type) {
	case EventClass::trigger_type::DEPENDENCY:
		return this->dependency_queue;

	case EventClass::trigger_type::DEPENDENCY_IMMEDIATELY:
		return this->dependency_immediately_queue;

	case EventClass::trigger_type::TRIGGER:
		return this->trigger_queue;

	case EventClass::trigger_type::REPEAT:
	case EventClass::trigger_type::ONCE:
	default:
		return this->event_queue;
	}
}


void EventQueue::update(const std::shared_ptr<Event> &evnt) {
	decltype(this->event_queue)::iterator it = std::find(std::begin(this->event_queue),
	                                                     std::end(this->event_queue),
	                                                     evnt);

	if (it != this->event_queue.end()) { // we only want to update the pending queue
		int dir = 0;

		{
			auto nxt = std::next(it);
			if (nxt != std::end(this->event_queue) and
			    (*nxt)->get_time() < (*it)->get_time()) {
				dir -= 1;
			}
			auto prv = std::prev(it);
			if (it != std::begin(this->event_queue) and
			    (*prv)->get_time() > (*it)->get_time()) {
				dir += 1;
			}
			// dir is now one of these values:
			// 0: the element is perfectly placed
			// +: the elements time is too low for its position, move forward
			// -: the elements time is too high for its position, move backwards
		}

		if (dir == 0) {
			// do nothing
		}
		else if (dir > 0) {
			auto e = *it;
			it = this->event_queue.erase(it);
			while (it != this->event_queue.begin()
			       && evnt->get_time() < (*std::prev(it))->get_time()) {
				it--;
			}
			this->event_queue.insert(it, e);
		}
		else if (dir < 0) {
			auto e = *it;
			it = this->event_queue.erase(it);
			while (it != this->event_queue.end()
			       && evnt->get_time() > (*std::prev(it))->get_time()) {
				it++;
			}
			this->event_queue.insert(it, e);
		}

		log::log(DBG << "Queue: readded Element: " << evnt->get_eventclass()->id()
		         << " at t=" << evnt->get_time());
	}
	else {
		it = std::find_if(
			std::begin(this->event_queue),
			std::end(this->event_queue),
			[&evnt] (const std::shared_ptr<Event> &e) {
				return e->get_time() > evnt->get_time();
			}
		);

		this->event_queue.insert(it, evnt);
		log::log(DBG << "Queue: inserted Element: " << evnt->get_eventclass()->id()
		         << " at t=" << evnt->get_time());
	}
}


void EventQueue::readd(const std::shared_ptr<Event> &evnt) {
	auto &container = this->select_queue(evnt->get_eventclass()->type);

	auto it = std::find_if(
		std::begin(container),
		std::end(container),
		[&evnt](const std::shared_ptr<Event> &element) {
			return element->get_time() < evnt->get_time();
		}
	);
	container.insert(it, evnt);
}


const std::deque<std::shared_ptr<Event>> &EventQueue::get_event_queue() const {
	return this->event_queue;
}


std::shared_ptr<Event> EventQueue::take_event(const curve::time_t &max_time) {
	if (this->event_queue.size() == 0) {
		return nullptr;
	}

	std::shared_ptr<Event> event = this->event_queue.front();

	// check if this event should be processed
	// we take any event that happens <= max_time
	if (event->get_time() <= max_time) {
		// remove the event from the queue
		this->event_queue.pop_front();

		return event;
	}
	else {
		return nullptr;
	}
}


const EventQueue::change_set &EventQueue::get_changes() const {
	return *this->changes;
}


void EventQueue::clear_changes() {
	this->changes->clear();
}


void EventQueue::swap_changesets() {
	std::swap(this->changes, this->future_changes);
}


EventQueue::OnChangeElement::OnChangeElement(const std::shared_ptr<Event> &evnt,
                                             const curve::time_t &time)
	:
	time{time},
	evnt{evnt},
	hash{evnt->hash()} {}


size_t EventQueue::OnChangeElement::Equal::operator()(const OnChangeElement& left,
                                                      const OnChangeElement& right) const {
	auto left_evnt = left.evnt.lock();
	auto right_evnt = right.evnt.lock();
	if (left_evnt && right_evnt) {
		if (left_evnt->get_eventclass()->id() == right_evnt->get_eventclass()->id()) {
			return true;
		}
	} else {
		return false;
	}

	auto left_trgt = left_evnt->get_target().lock();
	auto right_trgt = right_evnt->get_target().lock();

	return left_trgt && right_trgt &&
	left_trgt->id() == right_trgt->id();
}


} // namespace openage::event
