// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_ABILITY_H_
#define OPENAGE_UNIT_ABILITY_H_

#include <bitset>
#include <memory>

#include "../coord/phys3.h"

namespace openage {

class UnitAction;
class Texture;
class TestSound;
class Unit;

enum ability_type {
	move,
	patrol,
	gather,
	attack
};

using ability_set = std::bitset<8>;

/**
 * all bits set to 1
 */
const ability_set ability_all = ability_set().set();

/**
 * Abilities create an action when given a target
 * some abilities target positions such as moving or patroling
 * others target other game objects, such as attacking or
 * collecting relics
 */
class UnitAbility {
public:
	virtual ~UnitAbility() {}

	virtual ability_type type() = 0;

	virtual bool can_target(Unit *u1, coord::phys3 target) = 0;
	virtual bool can_target(Unit *u1, Unit *target) = 0;

	virtual std::shared_ptr<UnitAction> target(Unit *to_modify, coord::phys3 target) = 0;
	virtual std::shared_ptr<UnitAction> target(Unit *to_modify, Unit *target) = 0;
};

/*
 * initiates a move action when given a valid target
 */
class MoveAbility: public UnitAbility {
public:
	MoveAbility(Texture *t, TestSound *s);

	ability_type type() {
		return ability_type::move;
	}

	bool can_target(Unit *u1, coord::phys3 target);
	bool can_target(Unit *u1, Unit *target);

	std::shared_ptr<UnitAction> target(Unit *to_modify, coord::phys3 target);
	std::shared_ptr<UnitAction> target(Unit *to_modify, Unit *target);

private:
	Texture *tex;
	TestSound *sound;
};

/*
 * initiates an gather resource action when given a valid target
 */
class GatherAbility: public UnitAbility {
public:
	GatherAbility(Texture *t, TestSound *s);

	ability_type type() {
		return ability_type::gather;
	}

	bool can_target(Unit *u1, coord::phys3 target);
	bool can_target(Unit *u1, Unit *target);

	std::shared_ptr<UnitAction> target(Unit *to_modify, coord::phys3 target);
	std::shared_ptr<UnitAction> target(Unit *to_modify, Unit *target);

private:
	Texture *tex;
	TestSound *sound;
};

/*
 * initiates an attack action when given a valid target
 */
class AttackAbility: public UnitAbility {
public:
	AttackAbility(Texture *t, TestSound *s);

	ability_type type() {
		return ability_type::attack;
	}

	bool can_target(Unit *u1, coord::phys3 target);
	bool can_target(Unit *u1, Unit *target);

	std::shared_ptr<UnitAction> target(Unit *to_modify, coord::phys3 target);
	std::shared_ptr<UnitAction> target(Unit *to_modify, Unit *target);

private:
	Texture *tex;
	TestSound *sound;
};

} // namespace openage

#endif
