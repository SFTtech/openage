// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <functional>

#include "action.h"
#include "../engine.h"

namespace openage {
namespace input {

bool ActionManager::create(const std::string type) {
	if (this->actions.find(type) == this->actions.end()) {
		this->actions.insert(std::make_pair(type, this->actions.size()));
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

ActionManager::ActionManager() {
	for (auto &type : this->default_action) {
		this->create(type);
	}
}

}} // openage::input
