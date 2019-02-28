// Copyright 2017-2018 the openage authors. See copying.md for legal info.

#include "eventhandler.h"

#include "event.h"
#include "evententity.h"

#include "../log/log.h"

namespace openage::event {


EventHandler::EventHandler(const std::string &name, const EventHandler::trigger_type &type) :
	type{type},
	_id{name} {}


const std::string &EventHandler::id() {
	return _id;
}


DependencyEventHandler::DependencyEventHandler(const std::string &name)
	:
	EventHandler(name, EventHandler::trigger_type::DEPENDENCY) {}


DependencyImmediatelyEventHandler::DependencyImmediatelyEventHandler(const std::string &name)
	:
	EventHandler(name, EventHandler::trigger_type::DEPENDENCY_IMMEDIATELY) {}


TriggerEventHandler::TriggerEventHandler(const std::string &name)
	:
	EventHandler(name, EventHandler::trigger_type::TRIGGER) {}


RepeatEventHandler::RepeatEventHandler(const std::string &name)
	:
	EventHandler(name, EventHandler::trigger_type::REPEAT) {}


OnceEventHandler::OnceEventHandler(const std::string &name)
	:
	EventHandler(name, EventHandler::trigger_type::ONCE) {}


} // openage::event
