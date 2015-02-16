// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "command.h"

namespace openage {

Command::Command(Unit *unit, bool haspos, UnitProducer *producer)
	:
	has_pos{haspos},
	u{unit},
	type{producer} {
	this->modifiers.set();
}

Command::Command(Unit *unit)
	:
	Command{unit, false, nullptr} {
}

Command::Command(coord::phys3 position)
	:
	Command{nullptr, true, nullptr} {
	this->pos = position;
}

Command::Command(UnitProducer *producer)
	:
	Command{nullptr, false, producer} {
}

Command::Command(UnitProducer *producer, coord::phys3 position)
	:
	Command{nullptr, true, producer} {
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
