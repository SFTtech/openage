// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_ABILITY_H_
#define OPENAGE_UNIT_ABILITY_H_

#include <bitset>
#include <memory>
#include <type_traits>
#include <unordered_map>

#include "../coord/phys3.h"
#include "resource.h"

namespace openage {

class Command;
class Sound;
class Unit;
class UnitAction;
class UnitProducer;
class UnitTexture;

/**
 * roughly the same as command_ability in game data
 */
enum ability_type {
	move,
	patrol,
	train,
	build,
	research,
	gather,
	attack
};

struct ability_hash {
	size_t operator ()(const ability_type &t) const {
		return t;
	}
};

using ability_set = std::bitset<16>;
using ability_id_t = unsigned int;

/**
 * all bits set to 1
 */
const ability_set ability_all = ability_set().set();

/**
 * Abilities create an action when given a target
 * some abilities target positions such as moving or patroling
 * others target other game objects, such as attacking or
 * collecting relics
 *
 * Abilities are constructed with a default unit texture, but allow the texture
 * to be specified with the invoke function
 */
class UnitAbility {
public:
	virtual ~UnitAbility() {}

	virtual ability_type type() = 0;

	/** 
	 * true if the paramaters allow an action to be performed
	 */
	virtual bool can_invoke(Unit &to_modify, const Command &cmd) = 0;

 	/**
 	 * applys command to a given unit
 	 */
	virtual void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) = 0;
};

/*
 * initiates a move action when given a valid target
 */
class MoveAbility: public UnitAbility {
public:
	MoveAbility(Sound *s=nullptr);

	ability_type type() {
		return ability_type::move;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd);

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false);

private:
	Sound *sound;
};

/*
 * buildings train new objects
 */
class TrainAbility: public UnitAbility {
public:
	TrainAbility(Sound *s=nullptr);

	ability_type type() {
		return ability_type::train;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd);

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false);

private:
	Sound *sound;
};

/*
 * villagers build new buildings
 */
class BuildAbility: public UnitAbility {
public:
	BuildAbility(Sound *s=nullptr);

	ability_type type() {
		return ability_type::build;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd);

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false);

private:
	Sound *sound;
};

/*
 * initiates an gather resource action when given a valid target
 */
class GatherAbility: public UnitAbility {
public:
	GatherAbility(Sound *s=nullptr);

	ability_type type() {
		return ability_type::gather;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd);

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false);

private:
	Sound *sound;
};

/*
 * initiates an attack action when given a valid target
 */
class AttackAbility: public UnitAbility {
public:
	AttackAbility(Sound *s=nullptr);

	ability_type type() {
		return ability_type::attack;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd);

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false);

private:
	Sound *sound;
};

} // namespace openage

#endif
