// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#include "eventtarget.h"

#include "event.h"
#include "eventclass.h"
#include "loop.h"

#include "../log/log.h"


namespace openage::event {

void EventTarget::changes(const curve::time_t &time) {
	// This target has some change, so we have to notify all dependents
	// that subscribed on this target.

	log::log(DBG << "Target: change happens on " << this->id() << " at t=" << time);
	if (this->parent_notifier != nullptr) {
		this->parent_notifier(time);
	}

	// This is a maybe-erase construct so obsolete dependents are cleaned up.
	for (auto it = this->dependents.begin(); it != this->dependents.end(); ) {
		auto dependent = it->lock();
		if (dependent) {
			switch (dependent->get_eventclass()->type) {
			case EventClass::trigger_type::DEPENDENCY_IMMEDIATELY:
			case EventClass::trigger_type::DEPENDENCY:
				// Enqueue a change so that change events will be retriggered
				// (that depend on this target)
				// FIXME: Traverse the changed value
				this->loop->create_change(dependent, time, 0);
				++it;
				break;

			case EventClass::trigger_type::ONCE:
				// If the dependent is a ONCE-event
				// Has it been triggered? If so - forget it.
				if (dependent->last_triggered > 0) {
					it = this->dependents.erase(it);
				} else {
					// FIXME: Traverse the changed value
					this->loop->create_change(dependent, time, 0);
					++it;
				}
				break;

			case EventClass::trigger_type::TRIGGER:
			case EventClass::trigger_type::REPEAT:
				// Ignore announced changes for execute/keyframe events
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


void EventTarget::trigger(const curve::time_t &last_valid_time) {
	// notify all dependent events that are triggered `on_keyframe`
	// that the this target changed.
	// the only events that is "notified" by are TRIGGER.

	for (auto it = this->dependents.begin(); it != this->dependents.end(); ) {
		auto dependent = it->lock();
		if (dependent) {
			if (dependent->get_eventclass()->type == EventClass::trigger_type::TRIGGER) {
				log::log(DBG << "Target: trigger creates a change for "
				         << dependent->get_eventclass()->id()
				         << " at t=" << last_valid_time);

				// FIXME: Traverse the changed value
				loop->create_change(dependent, last_valid_time, 0);
			}

			++it;
		}
		else {
			it = this->dependents.erase(it);
		}
	}
}


void EventTarget::add_dependent(const std::weak_ptr<Event> &event) {
	this->dependents.emplace_back(event);
}

void EventTarget::show_dependents() const {
	log::log(DBG << "Dependent list:");
	for (auto &dep : this->dependents) {
		auto dependent = dep.lock();
		if (dependent) {
			log::log(DBG << " - " << dependent->get_eventclass()->id());
		}
		else {
			log::log(DBG << " - ** outdated old reference **");
		}
	}
}

} // openage::event
