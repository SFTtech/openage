// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "command.h"

namespace openage {

Command::Command(const Player &p, Unit *unit, bool haspos, UnitProducer *producer)
	:
	player(p),
	has_pos{haspos},
	u{unit},
	type{producer} {
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
}

Command::Command(const Player &p, UnitProducer *producer)
	:
	Command{p, nullptr, false, producer} {
}

Command::Command(const Player &p, UnitProducer *producer, coord::phys3 position)
	:
	Command{p, nullptr, true, producer} {
	this->pos = position;
}

bool Command::has_unit() const {
	return this->u;
}

bool Command::has_position() const {
	return this->has_pos;
}

bool Command::has_producer() const {
	return this->type;
}

Unit *Command::unit() const {
	return this->u;
}

coord::phys3 Command::position() const {
	return this->pos;
}

UnitProducer *Command::producer() const {
	return this->type;
}

void Command::set_ability(ability_type t) {
	this->modifiers = 0;
	this->modifiers[t] = true;
}

const ability_set &Command::ability() const {
	return this->modifiers;
}

} // namespace openage
