// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "input_context.h"

namespace openage {
namespace input {

InputContext::InputContext() {}

std::vector<std::string> InputContext::active_binds() const {
	std::vector<std::string> result;
	return result;
}

void InputContext::bind(action_t type, const action_func_t act) {
	by_type.emplace(std::make_pair(type, act));
}

void InputContext::bind(const Event &ev, const action_func_t act) {
	by_event.emplace(std::make_pair(ev, act));
}

void InputContext::bind(event_class ec, const action_func_t act) {
	by_class.emplace(std::make_pair(ec, act));
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
		if (action != this->by_class.end()) {
			action->second(arg);
			return true;
		}
	}

	return false;
}


} //namespace input
} //namespace openage
