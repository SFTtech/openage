// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_PLAYER_H_
#define OPENAGE_PLAYER_H_

#include <string>
#include <unordered_map>

#include "unit/resource.h"

namespace openage {

class Unit;

class Player {
public:
	Player(unsigned int number);

	const unsigned int player_number;

	/**
	 * would be better to have rgb color value
	 */
	const unsigned int color;
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

private:
	std::unordered_map<game_resource, double> resources;

};

} // openage

#endif
