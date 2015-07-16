// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "unit/unit.h"
#include "player.h"

namespace openage {

Player::Player(unsigned int number, std::string name)
	:
	player_number{number},
	color{number+1},
	civ{1},
	name{name} {
}

bool Player::is_enemy(const Player &other) const {

	// everyone else is enemy
	return player_number != other.player_number;
}

bool Player::is_ally(const Player &other) const {

	// everyone else is enemy
	return player_number == other.player_number;
}

bool Player::owns(Unit &unit) const {
	if (unit.has_attribute(attr_type::owner)) {
		return this == &unit.get_attribute<attr_type::owner>().player;
	}
	return false;
}

} // openage
