// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "command.h"

namespace openage {

Command::Command(const Player &p, Unit *unit, bool haspos, UnitType *t)
	:
	player(p),
	has_pos{haspos},
	u{unit},
	unit_type{t} {
	this->modifiers.set();
}

Command::Command(const Player &p, Unit *unit)
	:
	Command{p, unit, false, nullptr} {
}

Command::Command(const Player &p, coord::phys3 position)
	:
	Command{p, nullptr, true, nullptr} {
	this->pos = position;
}

Command::Command(const Player &p, Unit *unit, coord::phys3 position)
	:
	Command{p, unit, true, nullptr} {
	this->pos = position;

    //allow units to attack enemy sheep
    if ((unit->unit_type->unit_class == gamedata::unit_classes::SHEEP)
        && unit->unit_type->owner.is_enemy(p)) {

        this->add_flag(command_flag::attack_res);
    }
}

Command::Command(const Player &p, UnitType *t)
	:
	Command{p, nullptr, false, t} {
}

Command::Command(const Player &p, UnitType *t, coord::phys3 position)
	:
	Command{p, nullptr, true, t} {
	this->pos = position;
}

bool Command::has_unit() const {
	return this->u;
}

bool Command::has_position() const {
	return this->has_pos;
}

bool Command::has_type() const {
	return this->unit_type;
}

Unit *Command::unit() const {
	return this->u;
}

coord::phys3 Command::position() const {
	return this->pos;
}

UnitType *Command::type() const {
	return this->unit_type;
}

void Command::set_ability(ability_type t) {
	this->modifiers = 0;
	this->modifiers[static_cast<int>(t)] = true;
}

void Command::set_ability_set(ability_set set) {
	this->modifiers = set;
}

const ability_set &Command::ability() const {
	return this->modifiers;
}

void Command::add_flag(command_flag flag) {
	this->flags.insert(flag);
}

bool Command::has_flag(command_flag flag) const {
	return 0 < this->flags.count(flag);
}

} // namespace openage
