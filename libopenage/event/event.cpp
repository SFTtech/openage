// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include "event.h"

#include <utility>

#include "eventtarget.h"
#include "eventclass.h"

#include "../log/log.h"
#include "../util/hash.h"

namespace openage::event {

Event::Event(const std::shared_ptr<EventTarget> &target,
             const std::shared_ptr<EventClass> &eventclass,
             EventClass::param_map params)
	:
	params(std::move(params)),
	target{target},
	eventclass{eventclass},
	myhash{
		util::hash_combine(std::hash<size_t>()(target->id()),
		                   std::hash<std::string>()(eventclass->id()))
	} {}


void Event::depend_on(const std::shared_ptr<EventTarget> &dependency) {
	// TODO: do REPEAT and TRIGGER listen to changes (i.e. have dependents)?
	// if not, exclude them here and return early.

	log::log(DBG << "Adding change listener for: " << this->get_eventclass()->id()
	         << " to dependency with id=" << dependency->id());

	dependency->add_dependent(this->shared_from_this());
}


bool Event::operator <(const Event &other) const {
	return this->time < other.time;
}

} // openage::event
