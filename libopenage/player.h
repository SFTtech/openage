// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <unordered_map>

#include "unit/resource.h"

namespace openage {

class Unit;

class Player {
public:
	Player(unsigned int number, std::string name);

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
	 * civ index of this player
	 */
	const unsigned int civ;

	const std::string name;

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

private:
	std::unordered_map<game_resource, double> resources;

};

} // openage
