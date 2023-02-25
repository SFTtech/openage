// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "controller.h"

namespace openage::input {

Controller::Controller(const std::unordered_set<size_t> &controlled_factions,
                       size_t active_faction_id) :
	controlled_factions{controlled_factions},
	active_faction_id{active_faction_id} {}

void Controller::set_control(size_t faction_id) {
	if (this->controlled_factions.find(faction_id) != this->controlled_factions.end()) {
		this->active_faction_id = faction_id;
	}
}

size_t Controller::get_controlled() {
	return this->active_faction_id;
}

} // namespace openage::input
