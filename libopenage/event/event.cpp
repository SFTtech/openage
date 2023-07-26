// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#include "event.h"

#include <compare>
#include <functional>
#include <string>

#include "log/log.h"
#include "log/message.h"

#include "event/evententity.h"
#include "event/eventhandler.h"
#include "time/time.h"
#include "util/fixed_point.h"
#include "util/hash.h"


namespace openage::event {

Event::Event(const std::shared_ptr<EventEntity> &entity,
             const std::shared_ptr<EventHandler> &eventhandler,
             const EventHandler::param_map &params) :
	params(params),
	entity{entity},
	eventhandler{eventhandler},
	myhash{
		util::hash_combine(std::hash<size_t>()(entity->id()),
                           std::hash<std::string>()(eventhandler->id()))} {}


void Event::depend_on(const std::shared_ptr<EventEntity> &dependency) {
	// TODO: do REPEAT and TRIGGER listen to changes (i.e. have dependents)?
	// if not, exclude them here and return early.

	log::log(DBG << "Registering dependency event from EventHandler "
	             << this->get_eventhandler()->id()
	             << " to EventEntity " << dependency->idstr());

	dependency->add_dependent(this->shared_from_this());
}


void Event::cancel(const time::time_t reference_time) {
	log::log(DBG << "Canceling event from EventHandler "
	             << this->get_eventhandler()->id()
	             << " for time t=" << this->get_time());

	// remove the target which releases the event in the next loop iteration
	this->entity.reset();
	this->set_last_changed(reference_time);
}


bool Event::operator<(const Event &other) const {
	return this->time < other.time;
}

} // namespace openage::event
