// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include "event.h"

#include <utility>

#include "evententity.h"
#include "eventhandler.h"

#include "../log/log.h"
#include "../util/hash.h"

namespace openage::event {

Event::Event(const std::shared_ptr<EventEntity> &entity,
             const std::shared_ptr<EventHandler> &eventhandler,
             const EventHandler::param_map &params)
	:
	params(params),
	entity{entity},
	eventhandler{eventhandler},
	myhash{
		util::hash_combine(std::hash<size_t>()(entity->id()),
		                   std::hash<std::string>()(eventhandler->id()))
	} {}


void Event::depend_on(const std::shared_ptr<EventEntity> &dependency) {
	// TODO: do REPEAT and TRIGGER listen to changes (i.e. have dependents)?
	// if not, exclude them here and return early.

	log::log(DBG << "Registering dependency event from EventHandler "
	         << this->get_eventhandler()->id()
	         << " to EventEntity " << dependency->idstr());

	dependency->add_dependent(this->shared_from_this());
}


bool Event::operator <(const Event &other) const {
	return this->time < other.time;
}

} // openage::event
