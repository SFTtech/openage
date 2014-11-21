// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#include "../terrain/terrain_object.h"
#include "producer.h"
#include "unit.h"
#include "unit_container.h"

namespace openage {

UnitReference::UnitReference()
	:
	container{nullptr},
	unit_id{0},
	unit_ptr{nullptr} {}

UnitReference::UnitReference(const UnitContainer *c, id_t id, Unit *u)
	:
	container{c},
	unit_id{id},
	unit_ptr{u} {}

bool UnitReference::is_valid() const {
	return this->container && this->unit_ptr && this->container->valid_id(this->unit_id);
}

Unit *UnitReference::get() const {
	if (!this->is_valid()) {
		throw util::Error{"unit reference is no longer valid"};
	}
	return this->unit_ptr;
}

UnitContainer::UnitContainer()
	:
	next_new_id{0} {

}

UnitContainer::~UnitContainer() {
	for (auto &obj : this->live_units) {
		delete obj.second;
	}
}

bool UnitContainer::valid_id(id_t id) const {
	return (this->live_units.count(id) > 0);
}

UnitReference UnitContainer::get_unit(id_t id) {
	if (this->valid_id(id)) {
		return UnitReference(this, id, this->live_units[id]);
	}
	else {
		return UnitReference(this, id, nullptr);
	}
}

bool UnitContainer::new_unit(std::shared_ptr<UnitProducer> producer, Terrain *terrain, coord::tile tile) {
	Unit *newobj = new Unit(this, next_new_id++);

	// try creating a unit at this location
	bool placed = producer->place(newobj, terrain, tile);
	if (placed) {
		producer->initialise(newobj);
		this->live_units.insert({newobj->id, newobj});
	}
	else {
		delete newobj;
	}
	return placed;
}

bool dispatch_command(id_t, const Command &) {
	return true;
}

bool UnitContainer::on_tick() {
	// update everything and find objects with no actions
	std::vector<id_t> to_remove;
	for (auto &obj : this->live_units) {
		obj.second->update();
		if ( !obj.second->has_action() ) {
			to_remove.push_back(obj.first);
		}
	}

	// cleanup and removal of objects
	for (auto &obj : to_remove) {
		Unit *unit_ptr = this->live_units[obj];
		unit_ptr->location->remove();
		delete unit_ptr->location;
		delete unit_ptr;
		this->live_units.erase(obj);
	}
	return true;
}

} // namespace openage
