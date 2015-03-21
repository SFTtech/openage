// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include <memory>

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
	return this->unit_ptr &&
	       this->container->valid_id(this->unit_id);
}


Unit *UnitReference::get() const {
	if (!this->is_valid()) {
		throw util::Error{MSG(err) << "Unit reference is no longer valid"};
	}
	return this->unit_ptr;
}


UnitContainer::UnitContainer()
	:
	next_new_id{1} {
}


UnitContainer::~UnitContainer() {
	log::log(MSG(info) << "cleanup container");
}

void UnitContainer::reset() {
	this->live_units.clear();
}

void UnitContainer::set_terrain(Terrain *t) {
	this->terrain = t;
}

Terrain *UnitContainer::get_terrain() const {
	return this->terrain;
}


bool UnitContainer::valid_id(id_t id) const {
	return (this->live_units.count(id) > 0);
}


UnitReference UnitContainer::get_unit(id_t id) {
	if (this->valid_id(id)) {
		return UnitReference(this, id, this->live_units[id].get());
	}
	else {
		return UnitReference(this, id, nullptr);
	}
}

UnitReference UnitContainer::new_unit() {
	auto id = next_new_id++;
	this->live_units.emplace(id, std::make_unique<Unit>(*this, id));
	return this->live_units[id]->get_ref();
}

UnitReference UnitContainer::new_unit(UnitProducer &producer, Player &owner,
                             coord::phys3 position) {
	auto newobj = std::make_unique<Unit>(*this, next_new_id++);

	// try placing unit at this location
	bool placed = producer.place(newobj.get(), *this->terrain, position);
	if (placed) {
		producer.initialise(newobj.get(), owner);
		auto id = newobj->id;
		this->live_units.emplace(id, std::move(newobj));
		return this->live_units[id]->get_ref();
	}
	return UnitReference(); // is not valid
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

		// unique pointer triggers cleanup
		log::log(MSG(info) << "remove object " << obj);
		this->live_units.erase(obj);
	}
	return true;
}

std::vector<Unit *> UnitContainer::all_units() {
	std::vector<Unit *> result;
	for (auto &u : this->live_units) {
		result.push_back(u.second.get());
	}
	return result;
}

} // namespace openage
