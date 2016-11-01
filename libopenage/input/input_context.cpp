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
	// TODO: this function doesn't belong here.
	//       go give it a new home in another file.

	std::vector<std::string> result;

	if (this->input_manager == nullptr) {
		return result;
	}

	// this is only possible if the action is registered,
	// then this->input_manager != nullptr.
	ActionManager &action_manager = this->input_manager->get_engine()->get_action_manager();

	for (auto &action : this->by_type) {
		std::string keyboard_key;

		for (auto &key : this->input_manager->keys) {
			// TODO: this is the wrong check.
			//       the input manager should have a function
			//       which returns a list of binds when feeding
			//       in the InputContext.
			if (key.first == action.first) {
				keyboard_key = event_as_string(key.second);
				break;
			}
		}

		std::string action_type_str = action_manager.get_name(action.first);

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


void InputContext::register_to(InputManager *manager) {
	this->input_manager = manager;
}


void InputContext::unregister() {
	this->input_manager = nullptr;
}



}} // openage::input
