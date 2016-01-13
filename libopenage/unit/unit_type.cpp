// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "../terrain/terrain_object.h"
#include "action.h"
#include "unit.h"
#include "unit_type.h"

namespace openage {

UnitTexture *UnitType::default_texture() {
	return this->graphics[graphic_type::standing].get();
}

TerrainObject *UnitType::place_beside(Unit *u, TerrainObject const *other) const {
	if (!u || !other) {
		return nullptr;
	}

	// find the range of possible tiles
	tile_range outline{other->pos.start - coord::tile_delta{1, 1},
	                   other->pos.end   + coord::tile_delta{1, 1},
	                   other->pos.draw};

	// find a free position adjacent to the object
	auto terrain = other->get_terrain();
	for (coord::tile temp_pos : tile_list(outline)) {
		TerrainChunk *chunk = terrain->get_chunk(temp_pos);

		if (chunk == nullptr) {
			continue;
		}

		auto placed = this->place(u, terrain, temp_pos.to_phys2().to_phys3());
		if (placed) {
			return placed;
		}
	}
	return nullptr;
}

NyanType::NyanType() {
	// TODO: the type should be given attributes and abilities
}

NyanType::~NyanType() {}

int NyanType::id() const {
	return 1;
}

std::string NyanType::name() const {
	return "Nyan";
}

void NyanType::initialise(Unit *unit, Player &) {
	// reset any existing attributes and type
	unit->reset();

	// initialise unit
	unit->unit_type = this;
	unit->graphics = &this->graphics;

	// the parsed nyan data gives the list of attributes
	// and abilities which are given to the unit
	for (auto &ability : this->type_abilities) {
		unit->give_ability(ability);
	}

	// these are copied to the unit
	for (auto &attr : this->default_attributes) {
		unit->add_attribute(attr->copy());
	}

	// give idle action
	unit->push_action(std::make_unique<IdleAction>(unit), true);
}

TerrainObject *NyanType::place(Unit *unit, std::shared_ptr<Terrain> terrain, coord::phys3 init_pos) const {
	// the parsed nyan data gives the rules for terrain placement
	// which includes valid terrains, base radius and shape

	unit->make_location<SquareObject>(coord::tile_delta{1, 1});

	// allow unit to go anywhere
	unit->location->passable = [](const coord::phys3 &) {
		return true;
	};

	// try to place the obj, it knows best whether it will fit.
	if (unit->location->place(terrain, init_pos, object_state::placed)) {
		return unit->location.get();
	}

	// placing at the given position failed
	unit->log(MSG(dbg) << "failed to place object");
	return nullptr;
}

} /* namespace openage */
