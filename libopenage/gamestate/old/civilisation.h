// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "game_spec.h"

namespace openage {

/**
 * contains the initial tech structure for
 * one civilisation
 */
class Civilisation {
public:
	Civilisation(const GameSpec &spec, int id);

	/**
	 * civ index
	 */
	const int civ_id;

	/**
	 * civ name
	 */
	const std::string civ_name;

	/**
	 * return all the objects available to this civ
	 */
	std::vector<std::shared_ptr<UnitTypeMeta>> object_meta() const;

	/**
	 * return all types in a particular named category
	 */
	std::vector<index_t> get_category(const std::string &c) const;

	/**
	 * return all used categories, such as living, building or projectile
	 */
	std::vector<std::string> get_type_categories() const;

	/**
	 * gamedata for a building
	 */
	const gamedata::building_unit *get_building_data(index_t unit_id) const;

	/**
	 * initialise the unit meta data
	 */
	void initialise_unit_types(const GameSpec &spec);

private:

	/**
	 * creates and adds items to categories
	 */
	void add_to_category(const std::string &c, index_t type);

	/**
	 * unit types which can be produced by this civilisation.
	 */
	std::vector<std::shared_ptr<UnitTypeMeta>> civ_objects;

	/**
	 * all available categories of units
	 */
	std::vector<std::string> all_categories;

	/**
	 * category lists
	 */
	std::unordered_map<std::string, std::vector<index_t>> categories;

	/**
	 * used for annex creation
	 */
	std::unordered_map<index_t, const gamedata::building_unit *> buildings;

};

}
