// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "controller.h"

namespace openage::input {

Controller::Controller(const std::unordered_set<size_t> &controlled_factions,
                       size_t active_faction_id) :
	controlled_factions{controlled_factions},
	active_faction_id{active_faction_id},
	input_queue{} {}

void Controller::set_control(size_t faction_id) {
	if (this->controlled_factions.find(faction_id) != this->controlled_factions.end()) {
		this->active_faction_id = faction_id;
	}
}

size_t Controller::get_controlled() {
	return this->active_faction_id;
}

void Controller::process(const Event &ev) {
	// TODO
	// 1. Lookup input event
	// 2. take 1 of multiple actions:
	//   - queue input event (aka wait for more)
	//   - create gametate event (from all input events in queue)
	//   - clear queue (effectively cancel)
}

} // namespace openage::input
