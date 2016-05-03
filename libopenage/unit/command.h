// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_set>

#include "../coord/phys3.h"
#include "ability.h"

namespace openage {

/**
 * additional flags which may affect some abilities
 */
enum class command_flag {
	direct, // the user directly issued this command
	use_range, // move command account for units range
	attack_res // allow attack on a resource object
};

} // namespace openage

namespace std {

/**
 * hasher for game command flags
 */
template<>
struct hash<openage::command_flag> {
	typedef underlying_type<openage::command_flag>::type underlying_type;
	typedef hash<underlying_type>::result_type result_type;
	result_type operator()(const openage::command_flag &arg) const {
		hash<underlying_type> hasher;
		return hasher(static_cast<underlying_type>(arg));
	}
};

} // namespace std

namespace openage {

class Player;
class Unit;
class UnitType;

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
	Command(const Player &, UnitType *t);

	/**
	 * place building foundation
	 */
	Command(const Player &, UnitType *, coord::phys3);

	bool has_unit() const;
	bool has_position() const;
	bool has_type()const;

	Unit *unit() const;
	coord::phys3 position() const;
	UnitType *type() const;

	/**
	 * sets invoked ability type, no other type may be used if
	 * this gets set
	 *
	 * @param type allows a specific ability type to be used
	 * for example to set a unit to patrol rather than the default move
	 */
	void set_ability(ability_type t);

	/**
	 * restricts action to a set of possible ability types
	 */
	void set_ability_set(ability_set set);

	/**
	 * the ability types allowed to use this command
	 */
	const ability_set &ability() const;

	/**
	 * add addition option to this command
	 */
	void add_flag(command_flag flag);

	/**
	 * read the range setting
	 */
	bool has_flag(command_flag flag) const;

	/**
	 * player who created the command
	 */
	const Player &player;

private:

	/**
	 * basic constructor, which shouldnt be used directly
	 */
	Command(const Player &, Unit *unit, bool haspos, UnitType *t);

	bool has_pos;
	Unit *u;
	coord::phys3 pos;
	UnitType *unit_type;

	/**
	 * additional options
	 */
	std::unordered_set<command_flag> flags;

	/**
	 * select actions to use when targeting
	 */
	ability_set modifiers;

};

} // namespace openage
