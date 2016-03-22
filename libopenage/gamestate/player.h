// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <json/json.h>
#include <string>
#include <unordered_map>

#include "civilisation.h"
#include "resource.h"


namespace openage {

class Unit;

class Player {
public:
	Player(Civilisation *civ, unsigned int number, std::string name);
	Player(Json::Value);

	/**
	 * values 0 .. player count - 1
	 */
	const unsigned int player_number;

	/**
	 * values 1 .. player count
	 * would be better to have rgb color value
	 */
	const unsigned int color;

	/**
	 * civilisation and techs of this player
	 */
	const Civilisation *civ;

	/**
	 * visible name of this player
	 */
	const std::string name;

	/**
	 * checks if two players are the same
	 */
	bool operator ==(const Player &other) const;

	/**
	 * the specified player is an enemy of this player
	 */
	bool is_enemy(const Player &) const;

	/**
	 * the specified player is an ally of this player
	 */
	bool is_ally(const Player &) const;

	/**
	 * this player owns the specified unit
	 */
	bool owns(Unit &) const;

	/**
	 * add to stockpile
	 */
	void receive(const game_resource resource, double amount);

	/**
	 * remove from stockpile if available
	 * TODO parameter uses set of resources
	 */
	bool deduct(const game_resource resource, double amount);

	/**
	 * current stockpile amount
	 */
	double amount(const game_resource resource) const;

	/**
	 * total number of unit types available
	 */
	size_t type_count();

	/**
	 * unit types by aoe gamedata unit ids -- the unit type which corresponds to an aoe unit id
	 */
	UnitType *get_type(index_t type_id) const;

	/**
	 * unit types by list index -- a continuous array of all types
	 * probably not a useful function / can be removed
	 */
	UnitType *get_type_index(size_t type_index) const;

	/**
	 * initialise with the base tech level
	 */
	void initialise_unit_types();

	/**
	 * for savefile
	 */
	Json::Value toJson();

private:

	/**
	 * resources this player currently has
	 */
	std::unordered_map<game_resource, double> resources;

	/**
	 * unit types which can be produced by this player.
	 */
	unit_type_list available_objects;

	/**
	 * available objects mapped using type id
	 * unit ids -> unit type for that id
	 */
	std::unordered_map<index_t, UnitType *> available_ids;

};

} // openage
