// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <unordered_map>

#include "civilisation.h"
#include "population_tracker.h"
#include "resource.h"


namespace openage {

class Unit;
class Team;

class Player {
public:
	Player(Civilisation *civ, unsigned int number, std::string name);

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
	 * the team of this player
	 * nullptr if member of no team
	 */
	Team *team;

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
	void receive(const ResourceBundle& amount);
	void receive(const game_resource resource, double amount);

	/**
	 * remove from stockpile if available
	 */
	bool deduct(const ResourceBundle& amount);
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
	 * Keeps track of the population information.
	 */
	PopulationTracker population;

	/**
	 * Called when a unit is created and active.
	 * (Active means not a construction site)
	 */
	void active_unit_added(Unit *unit);

	/**
	 * Called when a unit is destroyed.
	 */
	void active_unit_removed(Unit *unit);

private:

	/**
	 * resources this player currently has
	 */
	ResourceBundle resources;

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
