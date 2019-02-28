// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include "evententity.h"

#include <string>

#include "event.h"
#include "eventhandler.h"
#include "loop.h"

#include "../log/log.h"


namespace openage::event {


void EventEntity::changes(const curve::time_t &time) {
	// This target has some change, so we have to notify all dependents
	// that subscribed on this entity.

	log::log(DBG << "Target: processing change request at t=" << time
	         << " for EventEntity " << this->idstr() << "...");
	if (this->parent_notifier != nullptr) {
		this->parent_notifier(time);
	}

	// This is a maybe-erase construct so obsolete dependents are cleaned up.
	for (auto it = this->dependents.begin(); it != this->dependents.end(); ) {
		auto dependent = it->lock();
		if (dependent) {
			switch (dependent->get_eventhandler()->type) {
			case EventHandler::trigger_type::DEPENDENCY_IMMEDIATELY:
			case EventHandler::trigger_type::DEPENDENCY:
				// Enqueue a change so that change events,
				// which depend on this target, will be retriggered

				log::log(DBG << "Target: change at t=" << time
				         << " for EventEntity " << this->idstr() << " registered");
				this->loop->create_change(dependent, time);
				++it;
				break;

			case EventHandler::trigger_type::ONCE:
				// If the dependent is a ONCE-event
				// forget the change if the once event has been notified already.
				if (dependent->get_last_changed() > curve::time_t::min_value()) {
					it = this->dependents.erase(it);
				} else {
					this->loop->create_change(dependent, time);
					++it;
				}
				break;

			case EventHandler::trigger_type::TRIGGER:
			case EventHandler::trigger_type::REPEAT:
				// Ignore announced changes for triggered or repeated events
				// for that there is the 'DEPENDENCY' events.
				++it;
				break;
			}
		}
		else {
			// The dependent is no more, so we can safely forget him
			it = this->dependents.erase(it);
		}
	}
}


void EventEntity::trigger(const curve::time_t &last_valid_time) {
	// notify all dependent events that are triggered `on_keyframe`
	// that the this target changed.
	// the only events that is "notified" by are TRIGGER.

	for (auto it = this->dependents.begin(); it != this->dependents.end(); ) {
		auto dependent = it->lock();
		if (dependent) {
			if (dependent->get_eventhandler()->type == EventHandler::trigger_type::TRIGGER) {
				log::log(DBG << "Target: trigger creates a change for "
				         << dependent->get_eventhandler()->id()
				         << " at t=" << last_valid_time);

				loop->create_change(dependent, last_valid_time);
			}

			++it;
		}
		else {
			it = this->dependents.erase(it);
		}
	}
}


void EventEntity::add_dependent(const std::shared_ptr<Event> &event) {
	switch (event->get_eventhandler()->type) {
	case EventHandler::trigger_type::TRIGGER:
	case EventHandler::trigger_type::REPEAT:
		throw Error(ERR << "Can't add a REPEAT or TRIGGER event '"
		            << event->get_eventhandler()->id()
		            << "' as dependent for EventEntity " << this->idstr());
		break;
	default:
		this->dependents.emplace_back(event);
		break;
	}
}

void EventEntity::show_dependents() const {
	log::log(DBG << "Dependent list:");
	for (auto &dep : this->dependents) {
		auto dependent = dep.lock();
		if (dependent) {
			log::log(DBG << " - " << dependent->get_eventhandler()->id());
		}
		else {
			log::log(DBG << " - ** outdated old reference **");
		}
	}
}

} // openage::event
