// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_ABILITY_H_
#define OPENAGE_UNIT_ABILITY_H_

#include <bitset>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "../coord/phys3.h"
#include "resource.h"

namespace openage {

class Command;
class Sound;
class Unit;
class UnitAction;
class UnitTexture;
class UnitType;

/**
 * roughly the same as command_ability in game data
 */
enum class ability_type {
	move,
	patrol,
	set_point,
	garrison,
	ungarrison,
	train,
	build,
	research,
	gather,
	attack,
	convert,
	repair,
	heal,
	MAX
};

/**
 * a containter where each ability uses 1 bit
 */
constexpr int ability_type_size = static_cast<int>(ability_type::MAX);
using ability_set = std::bitset<ability_type_size>;
using ability_id_t = unsigned int;

/**
 * all bits set to 1
 */
const ability_set ability_all = ability_set().set();

/**
 * the order abilities should be used when available
 */
static std::vector<ability_type> ability_priority {
	ability_type::gather,	// targeting
	ability_type::convert,
	ability_type::repair,
	ability_type::heal,
	ability_type::attack,
	ability_type::build,
	ability_type::move,	// positional
	ability_type::patrol,
	ability_type::garrison,
	ability_type::ungarrison, // inside buildings
	ability_type::train,
	ability_type::research,
	ability_type::set_point,
};


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
	 * applies command to a given unit
	 */
	virtual void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) = 0;

	/**
 	 * some common functions
	 */
	bool has_hitpoints(Unit &target);
	bool has_resource(Unit &target);
	bool is_same_player(Unit &to_modify, Unit &target);
	bool is_ally(Unit &to_modify, Unit &target);
	bool is_enemy(Unit &to_modify, Unit &target);

	/**
 	 * set bits corresponding to abilities, useful for initialising an ability_set
 	 * using a brace enclosed list
	 */
	static ability_set set_from_list(const std::vector<ability_type> &items);

};

/*
 * initiates a move action when given a valid target
 */
class MoveAbility: public UnitAbility {
public:
	MoveAbility(Sound *s=nullptr);

	ability_type type() override {
		return ability_type::move;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	Sound *sound;
};

/*
 * sets the gather point on buildings
 */
class SetPointAbility: public UnitAbility {
public:
	SetPointAbility();

	ability_type type() override {
		return ability_type::set_point;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;
};


/*
 * ability to garrision inside a building
 */
class GarrisonAbility: public UnitAbility {
public:
	GarrisonAbility(Sound *s=nullptr);

	ability_type type() override {
		return ability_type::garrison;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	Sound *sound;
};

/*
 * ability to ungarrision a building
 */
class UngarrisonAbility: public UnitAbility {
public:
	UngarrisonAbility(Sound *s=nullptr);

	ability_type type() override {
		return ability_type::ungarrison;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	Sound *sound;
};

/*
 * buildings train new objects
 */
class TrainAbility: public UnitAbility {
public:
	TrainAbility(Sound *s=nullptr);

	ability_type type() override {
		return ability_type::train;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	Sound *sound;
};

/*
 * villagers build new buildings
 */
class BuildAbility: public UnitAbility {
public:
	BuildAbility(Sound *s=nullptr);

	ability_type type() override {
		return ability_type::build;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	Sound *sound;
};

/*
 * initiates an gather resource action when given a valid target
 */
class GatherAbility: public UnitAbility {
public:
	GatherAbility(Sound *s=nullptr);

	ability_type type() override {
		return ability_type::gather;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	Sound *sound;
};

/*
 * initiates an attack action when given a valid target
 */
class AttackAbility: public UnitAbility {
public:
	AttackAbility(Sound *s=nullptr);

	ability_type type() override {
		return ability_type::attack;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	Sound *sound;
};

} // namespace openage

namespace std {

std::string to_string(const openage::ability_type &at);

/**
 * hasher for ability type enum
 */
template<>
struct hash<openage::ability_type> {
	typedef underlying_type<openage::ability_type>::type underlying_type;
	typedef hash<underlying_type>::result_type result_type;
	result_type operator()(const openage::ability_type &arg) const {
		hash<underlying_type> hasher;
		return hasher(static_cast<underlying_type>(arg));
	}
};

} // namespace std

#endif
