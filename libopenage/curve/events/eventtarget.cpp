// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "eventtarget.h"

#include "event.h"
#include "eventclass.h"
#include "eventmanager.h"

#include "../../error/error.h"
#include "../../log/log.h"

namespace openage {
namespace curve {

void EventTarget::on_change(const curve_time_t &time) {
	log::log(DBG << "Change happened on " << this->id() << " at " << time);
	if (parent_notifier != nullptr) {
		parent_notifier(time);
	}
	// Iterator advancement is done inside the loop - it is an maybe-erase construct
	for (auto it = dependents.begin(); it != dependents.end(); ) {
		auto sp = it->lock();
		if (sp) {
			switch (sp->eventclass()->type) {
			case EventClass::Type::ON_CHANGE_IMMEDIATELY:
			case EventClass::Type::ON_CHANGE:
				// Change events will be retriggered
				manager->changed(sp, time);
				++it;
				break;
			case EventClass::Type::ONCE:
				// Has it been triggered? If so - forget it.
				if (sp->last_triggered > 0) {
					it = dependents.erase(it);
				} else {
					manager->changed(sp, time);
					++it;
				}
				break;
			case EventClass::Type::ON_KEYFRAME:
			case EventClass::Type::ON_EXECUTE:
				// Ignore changes for execute events
				++it;
				break;
			}
		} else {
			// The dependent is no more, so we can safely forget him
			it = dependents.erase(it);
		}
	}
}


void EventTarget::on_pass_keyframe(const curve_time_t &last_valid_time) {
	for (auto it = dependents.begin(); it != dependents.end(); ) {
		auto sp = it->lock();
		if (sp) {
			if (sp->eventclass()->type == EventClass::Type::ON_KEYFRAME) {
				log::log(DBG << "Encountered keyframe " << sp->eventclass()->id()
				         << " at T:" << last_valid_time);
				manager->changed(sp, last_valid_time);
			}
			++it;
		} else {
			it = dependents.erase(it);
		}
	}
}


void EventTarget::add_dependent(const std::weak_ptr<Event> &event) {
	this->dependents.emplace_back(event);
}

}} // namespace openage::curve
