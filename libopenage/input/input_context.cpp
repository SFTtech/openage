// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "input_context.h"

#include "input_manager.h"


namespace openage {
namespace input {


InputContext::InputContext()
	:
	InputContext{nullptr} {}


InputContext::InputContext(InputManager *manager)
	:
	utf8_mode{false} {

	this->register_to(manager);
}


std::vector<std::string> InputContext::active_binds() const {
	if (this->input_manager == nullptr) {
		return {};
	}

	// TODO: try to purge this backpointer to the input manager.
	return this->input_manager->active_binds(this->by_type);
}

void InputContext::bind(action_t type, const action_func_t act) {
	this->by_type.emplace(std::make_pair(type, act));
}

void InputContext::bind(const Event &ev, const action_func_t act) {
	this->by_event.emplace(std::make_pair(ev, act));
}

void InputContext::bind(event_class ec, const action_check_t act) {
	this->by_class.emplace(std::make_pair(ec, act));
}

bool InputContext::execute_if_bound(const action_arg_t &arg) {

	// arg type hints are highest priority
	for (auto &h : arg.hints) {
		auto action = this->by_type.find(h);
		if (action != this->by_type.end()) {
			action->second(arg);
			return true;
		}
	}

	// specific event mappings
	auto action = this->by_event.find(arg.e);
	if (action != this->by_event.end()) {
		action->second(arg);
		return true;
	}

	// check all possible class mappings
	for (auto &c : arg.e.cc.get_classes()) {
		auto action = this->by_class.find(c);
		if (action != this->by_class.end() &&
		    action->second(arg)) {
			return true;
		}
	}

	return false;
}


void InputContext::register_to(InputManager *manager) {
	this->input_manager = manager;
}


void InputContext::unregister() {
	this->input_manager = nullptr;
}



}} // openage::input
