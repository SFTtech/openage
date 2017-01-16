// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "civilisation.h"

#include "../log/log.h"
#include "../unit/unit_type.h"


namespace openage {

Civilisation::Civilisation(const GameSpec &spec, int id)
	:
	civ_id{id},
	civ_name{spec.get_civ_name(id)} {
	this->initialise_unit_types(spec);
}


std::vector<std::shared_ptr<UnitTypeMeta>> Civilisation::object_meta() const {
	return civ_objects;
}


std::vector<index_t> Civilisation::get_category(const std::string &c) const {
	auto cat = this->categories.find(c);
	if (cat == this->categories.end()) {
		return std::vector<index_t>();
	}
	return cat->second;
}


std::vector<std::string> Civilisation::get_type_categories() const {
	return this->all_categories;
}


const gamedata::unit_building *Civilisation::get_building_data(index_t unit_id) const {
	if (this->buildings.count(unit_id) == 0) {
		log::log(MSG(info) << "  -> ignoring unit_id: " << unit_id);
		return nullptr;
	}
	return this->buildings.at(unit_id);
}


void Civilisation::initialise_unit_types(const GameSpec &spec) {
	log::log(MSG(dbg) << "Init units of civilisation " << civ_name);
	spec.create_unit_types(this->civ_objects, this->civ_id);
	for (auto &type : this->civ_objects) {
		this->add_to_category(type->name(), type->id());
	}
}


void Civilisation::add_to_category(const std::string &c, index_t type) {
	if (this->categories.count(c) == 0) {
		this->all_categories.push_back(c);
		this->categories[c] = std::vector<index_t>();
	}
	this->categories[c].push_back(type);
}


}
