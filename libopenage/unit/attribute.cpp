// Copyright 2016-2017 the openage authors. See copying.md for legal info.

#include "attribute.h"
#include "unit.h"
#include "unit_type.h"

namespace openage {

bool Attribute<attr_type::dropsite>::accepting_resource(game_resource res) const {
	return std::find(resource_types.begin(), resource_types.end(), res) != resource_types.end();
}

void Attribute<attr_type::multitype>::switchType(const gamedata::unit_classes cls, Unit *unit) const {
	auto search = this->types.find(cls);
	if (search != this->types.end()) {
		auto &player = unit->get_attribute<attr_type::owner>();
		search->second->reinitialise(unit, player.player);
	}
}

} // namespace openage
