// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "unit/unit.h"
#include "player.h"

namespace openage {

Player::Player(unsigned int number, std::string name)
	:
	player_number{number},
	color{number},
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

void Player::receive(const game_resource resource, double amount) {
	auto r = this->resources.find(resource);
	if (r == this->resources.end()) {
		this->resources[resource] = amount;
	}
	else {
		this->resources[resource] += amount;
	}
}

bool Player::deduct(const game_resource resource, double amount) {
	auto r = this->resources.find(resource);
	if (r == this->resources.end()) {
		return false;
	}
	else if (this->resources[resource] >= amount) {
		this->resources[resource] -= amount;
		return true;
	}
	return false;
}

double Player::amount(const game_resource resource) const {
	auto r = this->resources.find(resource);
	if (r == this->resources.end()) {
		return 0.0;
	}
	else {
		return this->resources.at(resource);
	}
}

} // openage
