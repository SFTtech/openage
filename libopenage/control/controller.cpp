// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#include "controller.h"

namespace openage::control {

Controller::Controller(/*Engine *engine,*/
                       /*GameMainHandle *game,*/
                       const std::unordered_set<size_t> controlled_factions,
                       size_t active_faction_id) :
	controlled_factions{controlled_factions},
	active_faction_id{active_faction_id} {}

void Controller::set_active_faction_id(size_t faction_id) {
	if (this->controlled_factions.find(faction_id) != this->controlled_factions.end()) {
		this->active_faction_id = faction_id;
	}
}

const size_t Controller::get_active_faction_id() const {
	return this->active_faction_id;
}

} // namespace openage::control