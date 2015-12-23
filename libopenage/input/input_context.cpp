// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "input_context.h"
#include "../engine.h"

namespace openage {
namespace input {

namespace {

std::string modSetString(modset_t mod) {
	if(mod.empty()) {
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

std::string eventAsString(const Event& event) {
	if(!event.as_utf8().empty()) {
		return modSetString(event.mod) + event.as_utf8();
	} else {
		if (event.cc.eclass == event_class::MOUSE_WHEEL) {
			if(event.cc.code == -1) {
				return modSetString(event.mod) + "Wheel down";
			} else {
				return modSetString(event.mod) + "Wheel up";
			}
		}
		return modSetString(event.mod) + SDL_GetKeyName(event.cc.code);
	}
}

} // anonymous ns

InputContext::InputContext()
	:
	utf8_mode{false} {}

std::vector<std::string> InputContext::active_binds() const {
	Engine& engine = Engine::get();
	InputManager& inputManager = engine.get_input_manager();

	std::vector<std::string> result;

	for(auto it = by_type.begin(); it != by_type.end(); it++) {
		std::string actionType{it->first.info, it->first.info + strlen(it->first.info)};
		std::string keyboardKey;
		for(auto it2 = inputManager.keys.begin(); it2 != inputManager.keys.end(); it2++) {
			if (it2->second.key == it->first.key) {
				keyboardKey = eventAsString(it2->first);
			}
		}

		result.push_back(keyboardKey + " : " + actionType);
	}

	return result;
}

void InputContext::bind(action_t type, const action_func_t act) {
	by_type.emplace(std::make_pair(type, act));
}

void InputContext::bind(const Event &ev, const action_func_t act) {
	by_event.emplace(std::make_pair(ev, act));
}

void InputContext::bind(event_class ec, const action_check_t act) {
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
		if (action != this->by_class.end() &&
		    action->second(arg)) {
			return true;
		}
	}

	return false;
}


}} // openage::input
