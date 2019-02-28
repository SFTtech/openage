// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include "eventqueue.h"

#include <algorithm>
#include <utility>

#include "event.h"
#include "eventhandler.h"
#include "evententity.h"
#include "../log/log.h"
#include "../util/compiler.h"


namespace openage::event {

std::shared_ptr<Event> EventQueue::create_event(const std::shared_ptr<EventEntity> &trgt,
                                                const std::shared_ptr<EventHandler> &cls,
                                                const std::shared_ptr<State> &state,
                                                const curve::time_t &reference_time,
                                                const EventHandler::param_map &params) {

	auto event = std::make_shared<Event>(trgt, cls, params);

	cls->setup_event(event, state);

	switch(cls->type) {
	case EventHandler::trigger_type::DEPENDENCY:
	case EventHandler::trigger_type::REPEAT:
	case EventHandler::trigger_type::ONCE:
		event->set_time(event->get_eventhandler()
		                     ->predict_invoke_time(trgt, state, reference_time));

		if (event->get_time() == std::numeric_limits<curve::time_t>::min()) {
			log::log(DBG << "Queue: ignoring insertion of event "
			         << event->get_eventhandler()->id() <<
			         " because no execution was scheduled.");

			return {};
		}
		break;

	case EventHandler::trigger_type::DEPENDENCY_IMMEDIATELY:
	case EventHandler::trigger_type::TRIGGER:
		event->set_time(reference_time);
		break;
	}

	log::log(DBG << "Queue: inserting event " << event->get_eventhandler()->id() <<
	         " into queue to be executed at t=" << event->get_time());

	// store the event
	// or enqueue it for execution
	switch(event->get_eventhandler()->type) {
	case EventHandler::trigger_type::DEPENDENCY:
		this->dependency_events.insert(event);
		break;

	case EventHandler::trigger_type::DEPENDENCY_IMMEDIATELY:
		this->dependency_immediately_events.insert(event);
		break;

	case EventHandler::trigger_type::TRIGGER:
		this->trigger_events.insert(event);
		break;

	case EventHandler::trigger_type::REPEAT:
	case EventHandler::trigger_type::ONCE:
	default:
		this->event_queue.push(event);
	}

	return event;
}


EventQueue::EventQueue()
	:
	changes(&changeset_A),
	future_changes(&changeset_B) {}


void EventQueue::add_change(const std::shared_ptr<Event> &event,
                            const curve::time_t &changed_at) {

	const curve::time_t event_previous_changed = event->get_last_changed();

	// Has the event already been fired in this round?
	if (event_previous_changed < changed_at) {
		auto it = this->changes->find(Change{event, changed_at});

		// Is the change already in the queue?
		if (it != changes->end()) {
			// Is the new change dated _before_ the old one?
			if (changed_at < it->time) {
				log::log(DBG << "Queue: adjusting time in change queue: moving event of "
				         << event->get_eventhandler()->id()
				         << " to earlier time");

				// Save the element
				Change change = *it;
				change.time = changed_at;

				// delete it from the container and readd it
				// with the updated time
				it = this->changes->erase(it);
				it = this->changes->insert(it, change);
			}
			else {
				// this change is to be ignored
				log::log(DBG << "Queue: skipping change for " << event->get_eventhandler()->id()
				         << " at " << changed_at
				         << " because there was already an earlier one at t=" << it->time);
			}
		}
		else {
			// the change was not in the to be changed list
			this->changes->emplace(event, changed_at);
			log::log(DBG << "Queue: inserting change for event from "
			         << event->get_eventhandler()->id()
			         << " to be applied at t=" << changed_at);
		}
	}
	else {
		// the event has been triggered in this round already, so skip it this time
		this->future_changes->emplace(event, changed_at);
		log::log(DBG << "Queue: ignoring change at t=" << changed_at
		         << " for event for handler " << event->get_eventhandler()->id()
		         << " because it's already processed as change at t=" << event_previous_changed);
	}

	event->set_last_changed(changed_at);
}


void EventQueue::remove(const std::shared_ptr<Event> &evnt) {
	// TODO: remove the event from the other storages.
	//       this would require changes to dependent events and triggers.
	//       (to stop being a dependent event or allow being triggered)
	this->event_queue.erase(evnt);
}


void EventQueue::enqueue(const std::shared_ptr<Event> &evnt) {
	if (this->event_queue.contains(evnt)) {
		this->event_queue.update(evnt);
	}
	else {
		this->event_queue.push(evnt);
	}
}


void EventQueue::reenqueue(const std::shared_ptr<Event> &evnt) {
	this->event_queue.push(evnt);
}


const EventStore &EventQueue::get_event_queue() const {
	return this->event_queue;
}


std::shared_ptr<Event> EventQueue::take_event(const curve::time_t &max_time) {
	if (this->event_queue.size() == 0) {
		return nullptr;
	}

	std::shared_ptr<Event> event = this->event_queue.top();

	// check if this event should be processed
	// we take any event that happens <= max_time
	if (event->get_time() <= max_time) {
		// remove the event from the queue
		this->event_queue.pop();

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


EventQueue::Change::Change(const std::shared_ptr<Event> &evnt,
                           curve::time_t time)
	:
	time{std::move(time)},
	evnt{evnt},
	hash{evnt->hash()} {}


size_t EventQueue::Change::Equal::operator()(const Change& left,
                                             const Change& right) const {
	auto left_evnt = left.evnt.lock();
	auto right_evnt = right.evnt.lock();
	if (left_evnt && right_evnt) {
		if (left_evnt->get_eventhandler()->id() == right_evnt->get_eventhandler()->id()) {
			return true;
		}
	} else {
		return false;
	}

	auto left_entity = left_evnt->get_entity().lock();
	auto right_entity = right_evnt->get_entity().lock();

	if (left_entity && right_entity) {
		if (left_entity->id() == right_entity->id()) {
			return true;
		}
	}

	return false;
}

} // namespace openage::event
