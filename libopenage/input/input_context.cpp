// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <sstream>
#include "input_context.h"
#include "../engine.h"

namespace openage {
namespace input {

namespace {

std::string mod_set_string(modset_t mod) {
	if (mod.empty()) {
		return "";
	} else {
		std::stringstream ss;
		for (auto it = mod.begin(); it != mod.end(); it++) {
				switch(*it) {
					case modifier::ALT:
						ss << "ALT + ";
						break;
					case modifier::CTRL:
						ss << "CTRL + ";
						break;
					case modifier::SHIFT:
						ss << "SHIFT + ";
						break;
				}
		}
		return ss.str();
	}
}

std::string event_as_string(const Event& event) {
	if (!event.as_utf8().empty()) {
		return mod_set_string(event.mod) + event.as_utf8();
	} else {
		if (event.cc.eclass == event_class::MOUSE_WHEEL) {
			if(event.cc.code == -1) {
				return mod_set_string(event.mod) + "Wheel down";
			} else {
				return mod_set_string(event.mod) + "Wheel up";
			}
		}
		return mod_set_string(event.mod) + SDL_GetKeyName(event.cc.code);
	}
}

} // anonymous ns

InputContext::InputContext()
	:
	utf8_mode{false} {}

std::vector<std::string> InputContext::active_binds() const {
	Engine &engine = Engine::get();
	InputManager &input_manager = engine.get_input_manager();
	ActionManager &action_manager = engine.get_action_manager();

	std::vector<std::string> result;

	for (auto &action : this->by_type) {
		std::string action_type_str = action_manager.get_name(action.first);

		std::string keyboard_key;
		for (auto &key : input_manager.keys) {
			if (key.second == action.first) {
				keyboard_key = event_as_string(key.first);
				break;
			}
		}

		result.push_back(keyboard_key + " : " + action_type_str);
	}

	return result;
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


}} // openage::input
