// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <bitset>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "../coord/phys.h"
#include "../gamestate/resource.h"

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
 * a container where each ability uses 1 bit
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
 * some abilities target positions such as moving or patrolling
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
	bool is_damaged(Unit &target);
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

/**
 * initiates a move action when given a valid target
 */
class MoveAbility: public UnitAbility {
public:
	MoveAbility(const Sound *s=nullptr);

	ability_type type() override {
		return ability_type::move;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	const Sound *sound;
};

/**
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


/**
 * ability to garrision inside a building
 */
class GarrisonAbility: public UnitAbility {
public:
	GarrisonAbility(const Sound *s=nullptr);

	ability_type type() override {
		return ability_type::garrison;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	const Sound *sound;
};

/**
 * ability to ungarrision a building
 */
class UngarrisonAbility: public UnitAbility {
public:
	UngarrisonAbility(const Sound *s=nullptr);

	ability_type type() override {
		return ability_type::ungarrison;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	const Sound *sound;
};

/**
 * buildings train new objects
 */
class TrainAbility: public UnitAbility {
public:
	TrainAbility(const Sound *s=nullptr);

	ability_type type() override {
		return ability_type::train;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	const Sound *sound;
};

/**
 * initiates a research
 */
class ResearchAbility: public UnitAbility {
public:
	ResearchAbility(const Sound *s=nullptr);

	ability_type type() override {
		return ability_type::research;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	const Sound *sound;
};

/**
 * villagers build new buildings
 */
class BuildAbility: public UnitAbility {
public:
	BuildAbility(const Sound *s=nullptr);

	ability_type type() override {
		return ability_type::build;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	const Sound *sound;
};

/**
 * initiates an gather resource action when given a valid target
 */
class GatherAbility: public UnitAbility {
public:
	GatherAbility(const Sound *s=nullptr);

	ability_type type() override {
		return ability_type::gather;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	const Sound *sound;
};

/**
 * initiates an attack action when given a valid target
 */
class AttackAbility: public UnitAbility {
public:
	AttackAbility(const Sound *s=nullptr);

	ability_type type() override {
		return ability_type::attack;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	const Sound *sound;
};

/**
 * initiates a repair action when given a valid target
 */
class RepairAbility: public UnitAbility {
public:
	RepairAbility(const Sound *s=nullptr);

	ability_type type() override {
		return ability_type::repair;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	const Sound *sound;
};

/**
 * initiates a heal action when given a valid target
 */
class HealAbility: public UnitAbility {
public:
	HealAbility(const Sound *s=nullptr);

	ability_type type() override {
		return ability_type::heal;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	const Sound *sound;
};

/**
 * initiates a patrol action when given a valid target
 * TODO implement
 */
class PatrolAbility: public UnitAbility {
public:
	PatrolAbility(const Sound *s=nullptr);

	ability_type type() override {
		return ability_type::patrol;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	const Sound *sound;
};

/**
 * initiates a convert action when given a valid target
 */
class ConvertAbility: public UnitAbility {
public:
	ConvertAbility(const Sound *s=nullptr);

	ability_type type() override {
		return ability_type::convert;
	}

	bool can_invoke(Unit &to_modify, const Command &cmd) override;

	void invoke(Unit &to_modify, const Command &cmd, bool play_sound=false) override;

private:
	const Sound *sound;
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

	size_t operator()(const openage::ability_type &arg) const {
		hash<underlying_type> hasher;
		return hasher(static_cast<underlying_type>(arg));
	}
};

} // namespace std
