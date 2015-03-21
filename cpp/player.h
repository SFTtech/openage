// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_PLAYER_H_
#define OPENAGE_PLAYER_H_

#include <string>
#include <unordered_map>

#include "unit/resource.h"

namespace openage {

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

	bool is_enemy(const Player &) const;
	bool is_ally(const Player &) const;

private:
	std::unordered_map<game_resource, double> resources;

};

}

#endif
