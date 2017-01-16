// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "player.h"

#include "../log/log.h"
#include "../unit/unit.h"
#include "../unit/unit_type.h"
#include "team.h"


namespace openage {

Player::Player(Civilisation *civ, unsigned int number, std::string name)
	:
	player_number{number},
	color{number},
	civ{civ},
	name{name},
	team{nullptr} {
	// starting resources
	this->resources[game_resource::food] = 1000;
	this->resources[game_resource::wood] = 1000;
	this->resources[game_resource::stone] = 1000;
	this->resources[game_resource::gold] = 1000;
}

bool Player::operator ==(const Player &other) const {
	return this->player_number == other.player_number;
}

bool Player::is_enemy(const Player &other) const {

	return !this->is_ally(other);
}

bool Player::is_ally(const Player &other) const {

	if (this->player_number == other.player_number) {
		return true; // same player
	}

	if (this->team && this->team->is_member(other)) {
		return true; // same team
	}

	// everyone else is enemy
	return false;
}

bool Player::owns(Unit &unit) const {
	if (unit.has_attribute(attr_type::owner)) {
		return this == &unit.get_attribute<attr_type::owner>().player;
	}
	return false;
}

void Player::receive(const ResourceBundle& amount) {
	this->resources += amount;
}

void Player::receive(const game_resource resource, double amount) {
	this->resources[resource] += amount;
}

bool Player::deduct(const ResourceBundle& amount) {
	return this->resources.deduct(amount);
}

bool Player::deduct(const game_resource resource, double amount) {
	if (this->resources[resource] >= amount) {
		this->resources[resource] -= amount;
		return true;
	}
	return false;
}

double Player::amount(const game_resource resource) const {
	return this->resources.get(resource);
}

size_t Player::type_count() {
	return this->available_ids.size();
}

UnitType *Player::get_type(index_t type_id) const {
	if (this->available_ids.count(type_id) == 0) {
		if (type_id > 0) {
			log::log(MSG(info) << "  -> ignoring type_id: " << type_id);
		}
		return nullptr;
	}
	return this->available_ids.at(type_id);
}

UnitType *Player::get_type_index(size_t type_index) const {
	if (type_index < available_objects.size()) {
		return available_objects.at(type_index).get();
	}
	log::log(MSG(info) << "  -> ignoring type_index: " << type_index);
	return nullptr;
}


void Player::initialise_unit_types() {
	log::log(MSG(info) << name << " has civilisation " << this->civ->civ_name);
	for (auto &type : this->civ->object_meta()) {
		auto shared_type = type->init(*this);
		index_t id = shared_type->id();
		this->available_objects.emplace_back(shared_type);
		this->available_ids[id] = shared_type.get();
	}
}


} // openage
