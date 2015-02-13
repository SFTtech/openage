// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_COMMAND_H_
#define OPENAGE_UNIT_COMMAND_H_

#include "producer.h"
#include "unit.h"

namespace openage {

/*
 * Game command from the ui
 */
class Command {
public:

	/**
	 * target another unit
	 */
	Command(Unit *);

	/**
	 * target a position
	 */
	Command(coord::phys3);

	/**
	 * select a type
	 */
	Command(UnitProducer *);

	/** 
	 * place building foundation
	 */ 
	Command(UnitProducer *, coord::phys3);

	/**
	 * the action can only be this ability
	 */
	void set_ability(ability_type);

private:
	Unit *unit;
	coord::phys3 position;
	UnitProducer *type;

	/**
	 * select actions to use when targeting
	 */
	ability_set modifiers;

};

} // namespace openage

#endif
