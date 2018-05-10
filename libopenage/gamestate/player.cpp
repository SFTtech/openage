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
	team{nullptr},
	population{0, 200}, // TODO change, get population cap max from game options
	score{this},
	age{1} { // TODO change, get starting age from game options
	// starting resources
	// TODO change, get starting resources from game options
	this->resources[game_resource::food] = 1000;
	this->resources[game_resource::wood] = 1000;
	this->resources[game_resource::stone] = 1000;
	this->resources[game_resource::gold] = 1000;
	this->on_resources_change();
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
	this->on_resources_change();
}

void Player::receive(const game_resource resource, double amount) {
	this->resources[resource] += amount;
	this->on_resources_change();
}

bool Player::deduct(const ResourceBundle& amount) {
	if (this->resources.deduct(amount)) {
		this->on_resources_change();
		return true;
	}
	return false;
}

bool Player::deduct(const game_resource resource, double amount) {
	if (this->resources[resource] >= amount) {
		this->resources[resource] -= amount;
		this->on_resources_change();
		return true;
	}
	return false;
}

bool Player::can_deduct(const ResourceBundle& amount) {
	return this->resources >= amount;
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

void Player::active_unit_added(Unit *unit) {
	// check if unit is actually active
	if (unit->has_attribute(attr_type::building) && unit->get_attribute<attr_type::building>().completed < 1.0f) {
		return;
	}

	this->units_have[unit->unit_type->id()] += 1;
	this->units_had[unit->unit_type->id()] += 1;
	// TODO handle here building dependencies

	// population
	if (unit->has_attribute(attr_type::population)) {
		auto popul = unit->get_attribute<attr_type::population>();
		if (popul.demand > 0) {
			this->population.demand_population(popul.demand);
		}
		if (popul.capacity > 0) {
			this->population.add_capacity(popul.capacity);
		}
	}

	// score
	// TODO improve selectors
	if (unit->unit_type->id() == 82 || unit->unit_type->id() == 276) { // Castle, Wonder
		this->score.add_score(score_category::society, unit->unit_type->cost.sum() * 0.2);
	} else if (unit->has_attribute(attr_type::building) || unit->has_attribute(attr_type::population)) { // building, living
		this->score.add_score(score_category::economy, unit->unit_type->cost.sum() * 0.2);
	}

	// TODO handle here on create unit triggers
	// TODO check for unit based win conditions
}

void Player::active_unit_removed(Unit *unit) {
	// check if unit is actually active
	if (unit->has_attribute(attr_type::building) && unit->get_attribute<attr_type::building>().completed < 1.0f) {
		return;
	}

	this->units_have[unit->unit_type->id()] -= 1;
	// TODO handle here building dependencies

	// population
	if (unit->has_attribute(attr_type::population)) {
		auto popul = unit->get_attribute<attr_type::population>();
		if (popul.demand > 0) {
			this->population.free_population(popul.demand);
		}
		if (popul.capacity > 0) {
			this->population.remove_capacity(popul.capacity);
		}
	}

	// score
	// TODO improve selectors
	if (unit->unit_type->id() == 82 || unit->unit_type->id() == 276) { // Castle, Wonder
		// nothing
	} else if (unit->has_attribute(attr_type::building) || unit->has_attribute(attr_type::population)) { // building, living
		this->score.remove_score(score_category::economy, unit->unit_type->cost.sum() * 0.2);
	}

	// TODO handle here on death unit triggers
	// TODO check for unit based win conditions
}

void Player::killed_unit(const Unit & unit) {
	// score
	this->score.add_score(score_category::military, unit.unit_type->cost.sum() * 0.2);
}

void Player::advance_age() {
	this->age += 1;
}

void Player::on_resources_change() {
	// score
	this->score.update_resources(this->resources);

	// TODO check for resource based win conditions
}

int Player::get_units_have(int type_id) const {
	if (this->units_have.count(type_id)) {
		return this->units_have.at(type_id);
	}
	return 0;
}

int Player::get_units_had(int type_id) const {
	if (this->units_had.count(type_id)) {
		return this->units_had.at(type_id);
	}
	return 0;
}

} // openage
