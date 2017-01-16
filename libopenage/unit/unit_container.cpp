// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "unit_container.h"

#include <memory>

#include "../log/log.h"
#include "../terrain/terrain_object.h"
#include "producer.h"
#include "unit.h"


namespace openage {

reference_data::reference_data(const UnitContainer *c, id_t id, Unit *u)
	:
	container{c},
	unit_id{id},
	unit_ptr{u} {
	if (!u) {
		throw Error{MSG(err) << "Cannot reference null unit pointer"};
	}
}


UnitReference::UnitReference()
	:
	data{nullptr} {}


UnitReference::UnitReference(const UnitContainer *c, id_t id, Unit *u)
	:
	data{std::make_shared<reference_data>(c, id, u)} {}


bool UnitReference::is_valid() const {
	return this->data &&
	       this->data->container->valid_id(this->data->unit_id);
}


Unit *UnitReference::get() const {
	if (!this->is_valid()) {
		throw Error{MSG(err) << "Unit reference is no longer valid"};
	}
	return this->data->unit_ptr;
}


UnitContainer::UnitContainer()
	:
	next_new_id{1} {}


UnitContainer::~UnitContainer() {
	log::log(MSG(warn) << "Cleanup container");
}

void UnitContainer::reset() {
	this->live_units.clear();
}

void UnitContainer::set_terrain(std::shared_ptr<Terrain> &t) {
	this->terrain = t;
}

std::shared_ptr<Terrain> UnitContainer::get_terrain() const {
	if (this->terrain.expired()) {
		throw Error{MSG(err) << "Terrain has expired"};
	}
	return this->terrain.lock();
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
	auto id = next_new_id;
	next_new_id += 1;

	this->live_units.emplace(id, std::make_unique<Unit>(this, id));
	return this->live_units[id]->get_ref();
}

UnitReference UnitContainer::new_unit(UnitType &type,
                                      Player &owner,
                                      coord::phys3 position) {

	auto new_id = next_new_id;
	next_new_id += 1;

	auto newobj = std::make_unique<Unit>(this, new_id);

	// try placing unit at this location
	auto terrain_shared = this->get_terrain();
	auto placed = type.place(newobj.get(), terrain_shared, position);
	if (placed) {
		type.initialise(newobj.get(), owner);
		auto id = newobj->id;
		this->live_units.emplace(id, std::move(newobj));
		return this->live_units[id]->get_ref();
	}
	return UnitReference(); // is not valid
}

UnitReference UnitContainer::new_unit(UnitType &type,
                                      Player &owner,
                                      TerrainObject *other) {
	auto new_id = next_new_id;
	next_new_id += 1;

	auto newobj = std::make_unique<Unit>(this, new_id);

	// try placing unit
	TerrainObject *placed = type.place_beside(newobj.get(), other);
	if (placed) {
		type.initialise(newobj.get(), owner);
		auto id = newobj->id;
		this->live_units.emplace(id, std::move(newobj));
		return this->live_units[id]->get_ref();
	}
	return UnitReference(); // is not valid
}


bool dispatch_command(id_t, const Command &) {
	return true;
}

bool UnitContainer::update_all(time_nsec_t lastframe_duration) {
	// update everything and find objects with no actions
	std::vector<id_t> to_remove;

	for (auto &obj : this->live_units) {
		obj.second->update(lastframe_duration);

		if (not obj.second->has_action()) {
			to_remove.push_back(obj.first);
		}
	}

	// cleanup and removal of objects
	for (auto &obj : to_remove) {

		// unique pointer triggers cleanup
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
