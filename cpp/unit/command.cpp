// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "command.h"

namespace openage {

Command::Command(Unit *unit)
	:
	has_pos{false},
	u{unit},
	type{nullptr} {
	this->modifiers.set();
}

Command::Command(coord::phys3 position)
	:
	has_pos{true},
	u{nullptr},
	pos(position),
	type{nullptr} {
	this->modifiers.set();
}

Command::Command(UnitProducer *producer)
	:
	has_pos{false},
	u{nullptr},
	type{producer} {
	this->modifiers.set();
}

Command::Command(UnitProducer *producer, coord::phys3 position)
	:
	has_pos{true},
	u{nullptr},
	pos(position),
	type{producer} {
	this->modifiers.set();
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
