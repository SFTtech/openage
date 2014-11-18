// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#include "producer.h"
#include "unit.h"
#include "unit_container.h"

namespace openage {

UnitContainer::UnitContainer()
	:
	next_new_id(0) {

}

UnitContainer::~UnitContainer() {
	for (auto &obj : this->live_units) {
		delete obj.second;
	}
}

bool UnitContainer::new_unit(std::shared_ptr<UnitProducer> producer, Terrain *terrain, coord::tile tile) {
	// try creating a unit at this location
	Unit *newobj = new Unit(next_new_id, producer);
	bool placed = producer->place(newobj, terrain, tile);
	if (placed) {
		this->live_units.insert({next_new_id++, newobj});
	}
	else {
		delete newobj;
	}
	return placed;
}

bool dispatch_command(uint, const Command &) {
	return true;
}

bool UnitContainer::on_tick() {
	// removal of units from the game
	std::vector<uint> to_remove;
	for (auto &obj : this->live_units) {
		obj.second->update();
		if ( !obj.second->has_action() ) {
			to_remove.push_back(obj.first);
		}
	}
	for (auto &obj : to_remove) {
		delete this->live_units[obj];
		this->live_units.erase(obj);
	}
	return true;
}

} // namespace openage
