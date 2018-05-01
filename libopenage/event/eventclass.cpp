// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#include "eventclass.h"

#include "event.h"
#include "eventtarget.h"

#include "../log/log.h"

namespace openage::event {


EventClass::EventClass(const std::string &name, const EventClass::trigger_type &type) :
	type{type},
	_id{name} {}


const std::string &EventClass::id() {
	return _id;
}


DependencyEventClass::DependencyEventClass(const std::string &name)
	:
	EventClass(name, EventClass::trigger_type::DEPENDENCY) {}


DependencyImmediatelyEventClass::DependencyImmediatelyEventClass(const std::string &name)
	:
	EventClass(name, EventClass::trigger_type::DEPENDENCY_IMMEDIATELY) {}


TriggerEventClass::TriggerEventClass(const std::string &name)
	:
	EventClass(name, EventClass::trigger_type::TRIGGER) {}


RepeatEventClass::RepeatEventClass(const std::string &name)
	:
	EventClass(name, EventClass::trigger_type::REPEAT) {}


OnceEventClass::OnceEventClass(const std::string &name)
	:
	EventClass(name, EventClass::trigger_type::ONCE) {}


} // openage::event
