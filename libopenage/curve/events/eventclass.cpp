// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "eventclass.h"

#include "event.h"
#include "eventtarget.h"

#include "../../log/log.h"

namespace openage {
namespace curve {

void EventClass::add_dependency (const std::shared_ptr<Event> &dependency,
                                 const std::shared_ptr<EventTarget> &element) {
	// ON_EXECUTE and ON_KEYFRAME do not listen on changes, so they do not have
	// any dependents.
	if (type != Type::ON_EXECUTE) {
		log::log(DBG << "Adding change listener for: " << dependency->eventclass()->id()
		         << " to " << element->id());
		element->add_dependent(dependency);
	}
}


EventClass::EventClass(const std::string &name, EventClass::Type type) :
	type(type),
	_id{name} {}


const std::string &EventClass::id() {
	return _id;
}

}} // namespace openage::curve
