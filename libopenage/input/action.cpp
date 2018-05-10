// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <functional>

#include "../cvar/cvar.h"
#include "action.h"
#include "input_manager.h"


namespace openage {
namespace input {

ActionManager::ActionManager(InputManager *input_manager,
                             cvar::CVarManager *cvar_manager)
	:
	input_manager{input_manager},
	cvar_manager{cvar_manager} {

	for (auto &type : this->default_action) {
		this->create(type);
	}
}

bool ActionManager::create(const std::string type) {
	if (this->actions.find(type) == this->actions.end()) {
		this->actions.insert(std::make_pair(type, this->actions.size()));

		// create the accessor of the action binding
		auto get = [type, this]() {
			return this->input_manager->get_bind(type);
		};
		auto set = [type, this](const std::string &value) {
			this->input_manager->set_bind(value.c_str(), type);
		};

		// and the corresponding cvar:
		// TODO: this has nothing to do with the actionmanager!
		//       remove the cvarmanager-access here!
		this->cvar_manager->create(type, std::make_pair(get, set));
		return true;
	}
	return false;
}

action_t ActionManager::get(const std::string &type) {
	auto it = this->actions.find(type);
	if (it != this->actions.end()) {
		return it->second;
	}
	return this->actions.at("UNDEFINED");
}

bool ActionManager::is(const std::string &type, const action_t action) {
	return get(type) == action;
}

std::string ActionManager::get_name(const action_t action) {
	for (auto &it : this->actions) {
		if (it.second == action) {
			return it.first;
		}
	}
	return "UNDEFINED";
}

}} // openage::input
