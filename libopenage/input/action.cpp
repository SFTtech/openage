// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <functional>

#include "action.h"
#include "../engine.h"

namespace openage {
namespace input {

bool ActionManager::create(const std::string type) {
	if (this->actions.find(type) == this->actions.end()) {
		this->actions.insert(std::make_pair(type, this->actions.size()));

		// create the accessor of the action binding
		auto get = [type, this]() {
			return this->engine->get_input_manager().get_bind(type);
		};
		auto set = [type, this](const std::string &value) {
			this->engine->get_input_manager().set_bind(value.c_str(), type);
		};
		// and the corresponding cvar
		this->engine->get_cvar_manager().create(type, std::make_pair(get,set));
		return true;
	}
	return false;
}

action_t ActionManager::get(const std::string &type) const {
	auto it = this->actions.find(type);
	if (it != this->actions.end()) {
		return it->second;
	}
	return this->actions.at("UNDEFINED");
}

bool ActionManager::is(const std::string &type, const action_t action) {
	return get(type) == action;
}

std::string ActionManager::get_name(const action_t action) const {
	for (auto &it : this->actions) {
		if (it.second == action) {
			return it.first;
		}
	}
	return "UNDEFINED";
}

ActionManager::ActionManager(Engine *e): engine(e) {
	for (auto &type : this->default_action) {
		this->create(type);
	}
}

}} // openage::input
