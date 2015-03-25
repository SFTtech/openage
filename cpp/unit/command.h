// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_COMMAND_H_
#define OPENAGE_UNIT_COMMAND_H_

#include "../coord/phys3.h"
#include "ability.h"
#include "producer.h"
#include "unit.h"

namespace openage {

class Player;

/*
 * Game command from the ui
 */
class Command {
public:

	/**
	 * target another unit
	 */
	Command(const Player &, Unit *unit);

	/**
	 * target a position
	 */
	Command(const Player &, coord::phys3 position);

	/**
	 * target another unit or a position
	 */
	Command(const Player &, Unit *unit, coord::phys3 position);

	/**
	 * select a type
	 */
	Command(const Player &, UnitProducer *producer);

	/** 
	 * place building foundation
	 */ 
	Command(const Player &, UnitProducer *, coord::phys3);

	bool has_unit() const;
	bool has_position() const;
	bool has_producer()const;

	Unit *unit() const;
	coord::phys3 position() const;
	UnitProducer *producer() const;

	/**
	 * the action can only be this ability
	 *
	 * @param type allows a specific ability type to be used
	 * for example to set a unit to patrol rather than the default move
	 */
	void set_ability(ability_type t);

	/**
	 * setting to true makes a move command account for units range,
	 * when false a unit moves as near as possible to a target.
	 */
	void set_range(bool r);

	/**
	 * the ability types allowed to use this command
	 */
	const ability_set &ability() const;

	/**
	 * read the range setting
	 */
	bool is_ranged() const;

	/**
	 * player who created the command
	 */
	const Player &player;

private:

	/**
	 * basic constructor, which shouldnt be used directly
	 */
	Command(const Player &, Unit *unit, bool haspos, UnitProducer *producer);

	bool has_pos;
	Unit *u;
	coord::phys3 pos;
	UnitProducer *type;

	/**
	 * modified units will use their range parameter
	 */
	bool use_range;

	/**
	 * select actions to use when targeting
	 */
	ability_set modifiers;

};

} // namespace openage

#endif
