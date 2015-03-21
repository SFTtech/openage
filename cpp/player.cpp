// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "player.h"

namespace openage {

Player::Player(unsigned int number)
	:
	player_number{number},
	color{number+1},
	civ{1},
	name{"player " + std::to_string(color)} {
}

bool Player::is_enemy(const Player &other) const {

	// everyone else is enemy
	return player_number != other.player_number;
}

bool Player::is_ally(const Player &other) const {

	// everyone else is enemy
	return player_number == other.player_number;
}

}
