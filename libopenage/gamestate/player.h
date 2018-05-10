// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <unordered_map>

#include "civilisation.h"
#include "population_tracker.h"
#include "resource.h"
#include "score.h"


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
	 * Check if the player has enough resources to deduct the given amount.
	 */
	bool can_deduct(const ResourceBundle& amount);

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
	 * The score of the player.
	 */
	PlayerScore score;

	/**
	 * Called when a unit is created and active.
	 * (Active means not a construction site)
	 */
	void active_unit_added(Unit *unit);

	/**
	 * Called when a unit is destroyed.
	 */
	void active_unit_removed(Unit *unit);

	/**
	 * Called when a unit is killed by this player.
	 */
	void killed_unit(const Unit & unit);

	/**
	 * Advance to next age;
	 */
	void advance_age();

	// Getters

	/**
	 * Get the number of units the player has for each unit type id.
	 */
	int get_units_have(int type_id) const;

	/**
	 * Get the number of units the player ever had for each unit type id.
	 */
	int get_units_had(int type_id) const;

	/**
	 * Get the current age.
	 * The first age has the value 1.
	 */
	int get_age() const { return age; }

private:

	/**
	 * resources this player currently has
	 */
	ResourceBundle resources;

	/**
	 * Called when the resources amounts change.
	 */
	void on_resources_change();

	/**
	 * unit types which can be produced by this player.
	 * TODO revisit, can be simplified?
	 */
	unit_type_list available_objects;

	/**
	 * available objects mapped using type id
	 * unit ids -> unit type for that id
	 * TODO revisit, can be simplified?
	 */
	std::unordered_map<index_t, UnitType *> available_ids;

	/**
	 * The number of units the player has for each unit type id.
	 * Used for unit dependencies (eg. Town Center) and event triggers.
	 */
	std::unordered_map<int, int> units_have;

	/**
	 * The number of units the player ever had for each unit type id.
	 * Used for unit dependencies (eg. Farm).
	 */
	std::unordered_map<int, int> units_had;

	/**
	 * The current age.
	 */
	int age;

};

} // openage
