// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "player.h"

#include <utility>

#include "../../log/log.h"
#include "../../unit/unit.h"
#include "../../unit/unit_type.h"
#include "../../util/math_constants.h"
#include "team.h"


namespace openage {

Player::Player(Civilisation *civ, unsigned int number, std::string name)
	:
	player_number{number},
	color{number},
	civ{civ},
	name{std::move(name)},
	team{nullptr},
	population{0, 200}, // TODO change, get population cap max from game options
	score{this},
	age{1} { // TODO change, get starting age from game options
	// starting resources
	// TODO change, get starting resources from game options
	this->resources.set_all(1000);
	// TODO change, get starting resources capacity from game options or nyan
	this->resources_capacity.set_all(math::DOUBLE_INF / 2); // half to avoid overflows
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

bool Player::can_receive(const ResourceBundle& amount) const {
	return this->resources_capacity.has(this->resources, amount);
}

bool Player::can_receive(const game_resource resource, double amount) const {
	return this->resources_capacity.get(resource) >= this->resources.get(resource) + amount;
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

bool Player::can_deduct(const ResourceBundle& amount) const {
	return this->resources.has(amount);
}

bool Player::can_deduct(const game_resource resource, double amount) const {
	return this->resources.get(resource) >= amount;
}

double Player::amount(const game_resource resource) const {
	return this->resources.get(resource);
}

bool Player::can_make(const UnitType &type) const {
	return this->can_deduct(type.cost.get(*this)) &&
	       this->get_units_have(type.id()) + this->get_units_pending(type.id()) < type.have_limit &&
	       this->get_units_had(type.id()) + this->get_units_pending(type.id()) < type.had_limit;
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

void Player::active_unit_added(Unit *unit, bool from_pending) {
	// check if unit is actually active
	if (this->is_unit_pending(unit)) {
		this->units_pending[unit->unit_type->id()] += 1;
		return;
	}

	if (from_pending) {
		this->units_pending[unit->unit_type->id()] -= 1;
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

	// resources capacity
	if (unit->has_attribute(attr_type::storage)) {
		auto storage = unit->get_attribute<attr_type::storage>();
		this->resources_capacity += storage.capacity;
		this->on_resources_change();
	}

	// score
	// TODO improve selectors
	if (unit->unit_type->id() == 82 || unit->unit_type->id() == 276) { // Castle, Wonder
		this->score.add_score(score_category::society, unit->unit_type->cost.get(*this).sum() * 0.2);
	} else if (unit->has_attribute(attr_type::building) || unit->has_attribute(attr_type::population)) { // building, living
		this->score.add_score(score_category::economy, unit->unit_type->cost.get(*this).sum() * 0.2);
	}

	// TODO handle here on create unit triggers
	// TODO check for unit based win conditions
}

void Player::active_unit_removed(Unit *unit) {
	// check if unit is actually active
	if (this->is_unit_pending(unit)) {
		this->units_pending[unit->unit_type->id()] -= 1;
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

	// resources capacity
	if (unit->has_attribute(attr_type::storage)) {
		auto storage = unit->get_attribute<attr_type::storage>();
		this->resources_capacity -= storage.capacity;
		this->on_resources_change();
	}

	// score
	// TODO improve selectors
	if (unit->unit_type->id() == 82 || unit->unit_type->id() == 276) { // Castle, Wonder
		// nothing
	} else if (unit->has_attribute(attr_type::building) || unit->has_attribute(attr_type::population)) { // building, living
		this->score.remove_score(score_category::economy, unit->unit_type->cost.get(*this).sum() * 0.2);
	}

	// TODO handle here on death unit triggers
	// TODO check for unit based win conditions
}

void Player::killed_unit(const Unit & unit) {
	// score
	this->score.add_score(score_category::military, unit.unit_type->cost.get(*this).sum() * 0.2);
}

void Player::advance_age() {
	this->age += 1;
}

void Player::on_resources_change() {

	// capacity overflow
	if (! (this->resources_capacity >= this->resources_capacity)) {
		this->resources.limit(this->resources_capacity);
	}

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

int Player::get_units_pending(int type_id) const {
	if (this->units_pending.count(type_id)) {
		return this->units_pending.at(type_id);
	}
	return 0;
}

bool Player::is_unit_pending(Unit *unit) const {
	// TODO check aslo if unit is training
	return unit->has_attribute(attr_type::building) && unit->get_attribute<attr_type::building>().completed < 1.0f;
}

int Player::get_workforce_count() const {
	// TODO get all units tagged as work force
	return this->units_have.at(83) + this->units_have.at(293) + // villagers
	       this->units_have.at(13) + // fishing ship
	       this->units_have.at(128) + // trade cart
	       this->units_have.at(545); // transport ship
}

} // openage
