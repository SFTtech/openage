// Copyright 2014-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <map>
#include <algorithm>

#include "../coord/tile.h"
#include "../gamedata/unit.gen.h"
#include "../terrain/terrain_object.h"
#include "../gamestate/old/resource.h"
#include "unit_container.h"

namespace std {

/**
 * hasher for unit classes enum type
 */
template<> struct hash<openage::gamedata::unit_classes> {
	typedef underlying_type<openage::gamedata::unit_classes>::type underlying_type;

	size_t operator()(const openage::gamedata::unit_classes &arg) const {
		hash<underlying_type> hasher;
		return hasher(static_cast<underlying_type>(arg));
	}
};

} // namespace std

namespace openage {

/**
 * Types of action graphics
 */
enum class graphic_type {
	construct,
	shadow,
	decay,
	dying,
	standing,
	walking,
	carrying,
	attack,
	heal,
	work
};

class UnitTexture;

/**
 * Collection of graphics attached to each unit.
 */
using graphic_set = std::map<graphic_type, std::shared_ptr<UnitTexture>>;

/**
 * List of unit's attribute types.
 */
enum class attr_type {
	owner,
	population,
	damaged,
	hitpoints,
	armor,
	attack,
	formation,
	heal,
	speed,
	direction,
	projectile,
	building,
	dropsite,
	resource,
	resource_generator,
	worker,
	storage,
	multitype,
	garrison
};

/**
 * List of unit's attack stance.
 * Can be used for buildings also.
 */
enum class attack_stance {
	aggressive,
	defensive,
	stand_ground,
	do_nothing
};

/**
 * List of unit's formation.
 * Effect applys on a group of units.
 */
enum class attack_formation {
	line,
	staggered,
	box,
	flank
};


/**
 * this type gets specialized for each attribute
 */
template<attr_type T> class Attribute;

/**
 * Wraps a templated attribute
 */
class AttributeContainer {
public:
	AttributeContainer() {}

	AttributeContainer(attr_type t)
		:
		type{t} {}

	virtual ~AttributeContainer() = default;

	attr_type type;

	/**
	 * shared attributes are common across all units of
	 * one type, such as max hp, and gather rates
	 *
	 * non shared attributes include a units current hp,
	 * and the amount a villager is carrying
	 */
	virtual bool shared() const = 0;

	/**
	 * Produces an copy of the attribute.
	 */
	virtual std::shared_ptr<AttributeContainer> copy() const = 0;
};

/**
 * An unordered_map with a int key used as a type id
 * and a unsigned int value used as the amount
 */
using typeamount_map = std::unordered_map<int, unsigned int>;

/**
 * Wraps a templated shared attribute
 *
 * Shared attributes are common across all units of
 * one type
 */
class SharedAttributeContainer: public AttributeContainer {
public:

	SharedAttributeContainer(attr_type t)
		:
		AttributeContainer{t} {}

	bool shared() const override {
		return true;
	}
};

/**
 * Wraps a templated unshared attribute
 *
 * Shared attributes are copied for each unit of
 * one type
 */
class UnsharedAttributeContainer: public AttributeContainer {
public:

	UnsharedAttributeContainer(attr_type t)
		:
		AttributeContainer{t} {}

	bool shared() const override {
		return false;
	}
};

// -----------------------------
// attribute definitions go here
// -----------------------------

class Player;

template<> class Attribute<attr_type::owner>: public SharedAttributeContainer {
public:
	Attribute(Player &p)
		:
		SharedAttributeContainer{attr_type::owner},
		player(p) {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::owner>>(*this);
	}

	Player &player;
};

/**
 * The max hitpoints and health bar information.
 * TODO change bar information stucture
 */
template<> class Attribute<attr_type::hitpoints>: public SharedAttributeContainer {
public:
	Attribute(unsigned int i)
		:
		SharedAttributeContainer{attr_type::hitpoints},
		hp{i} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::hitpoints>>(*this);
	}

	/**
	 * The max hitpoints
	 */
	unsigned int hp;
	float hp_bar_height;
};

/**
 * The population capacity and the population demand.
 */
template<> class Attribute<attr_type::population>: public SharedAttributeContainer {
public:
	Attribute(int demand, int capacity)
		:
		SharedAttributeContainer{attr_type::population},
		demand{demand},
		capacity{capacity} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::population>>(*this);
	}

	int demand;
	int capacity;
};

/**
 * The current hitpoints.
 * TODO add last damage taken timestamp
 */
template<> class Attribute<attr_type::damaged>: public UnsharedAttributeContainer {
public:
	Attribute(unsigned int i)
		:
		UnsharedAttributeContainer{attr_type::damaged},
		hp{i} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::damaged>>(*this);
	}

	/**
	 * The current hitpoint
	 */
	unsigned int hp;
};

template<> class Attribute<attr_type::armor>: public SharedAttributeContainer {
public:
	Attribute(typeamount_map a)
		:
		SharedAttributeContainer{attr_type::armor},
		armor{a} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::armor>>(*this);
	}

	typeamount_map armor;
};

/**
 * TODO can a unit have multiple attacks such as villagers hunting map target classes onto attacks
 * TODO remove the first constructor and the default values after (keep for now for compatibility)
 */
template<> class Attribute<attr_type::attack>: public SharedAttributeContainer {
public:
	// TODO remove (keep for testing)
	// 4 = gamedata::hit_class::UNITS_MELEE (not exported at the moment)
	Attribute(UnitType *type, coord::phys_t r, coord::phys_t h, unsigned int d)
		:
		Attribute{type, r, h, {{4, d}}} {}

	Attribute(UnitType *type, coord::phys_t r, coord::phys_t h, typeamount_map d,
	          coord::phys_t min_range=0, bool friendly_fire=false,
	          coord::phys_t area_of_effect=0)
		:
		SharedAttributeContainer{attr_type::attack},
		ptype{type},
		min_range{min_range},
		max_range{r},
		init_height{h},
		damage{d},
		friendly_fire{friendly_fire},
		area_of_effect{area_of_effect} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::attack>>(*this);
	}

	/**
	 * The projectile's unit type
	 */
	UnitType *ptype;

	/**
	 * The min range of the attack
	 * TODO not used
	 */
	coord::phys_t min_range;

	/**
	 * The max range of the attack
	 */
	coord::phys_t max_range;

	/**
	 * The height from which the projectile starts
	 */
	coord::phys_t init_height;

	typeamount_map damage;

	/**
	 * If the attack can damage allied (friendly) units.
	 * TODO not used
	 */
	bool friendly_fire;

	/**
	 * The radius of the area of effect of the attack or 0 if there is no area_of_effect.
	 * TODO not used
	 */
	coord::phys_t area_of_effect;
};

/**
 * The attack stance and formation
 * TODO store patrol and follow command information
 */
template<> class Attribute<attr_type::formation>: public UnsharedAttributeContainer {
public:

	Attribute()
		:
		Attribute{attack_stance::do_nothing} {}

	Attribute(attack_stance stance)
		:
		UnsharedAttributeContainer{attr_type::formation},
		stance{stance},
		formation{attack_formation::line} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::formation>>(*this);
	}

	attack_stance stance;
	attack_formation formation;
};

/**
 * Healing capabilities.
 */
template<> class Attribute<attr_type::heal>: public SharedAttributeContainer {
public:
	Attribute(coord::phys_t r, unsigned int l, unsigned int ra)
		:
		SharedAttributeContainer{attr_type::heal},
		range{r},
		life{l},
		rate{ra} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::heal>>(*this);
	}

	/**
	 * The max range of the healing.
	 */
	coord::phys_t range;

	/**
	 * Life healed in each cycle
	 */
	unsigned int life;

	/**
	 * The period of each heal cycle
	 */
	unsigned int rate;
};

template<> class Attribute<attr_type::speed>: public SharedAttributeContainer {
public:
	Attribute(coord::phys_t sp)
		:
		SharedAttributeContainer{attr_type::speed},
		unit_speed{sp} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::speed>>(*this);
	}

	// TODO rename to default or normal
	coord::phys_t unit_speed;
};

template<> class Attribute<attr_type::direction>: public UnsharedAttributeContainer {
public:
	Attribute(coord::phys3_delta dir)
		:
		UnsharedAttributeContainer{attr_type::direction},
		unit_dir(dir) {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::direction>>(*this);
	}

	coord::phys3_delta unit_dir;
};

template<> class Attribute<attr_type::projectile>: public UnsharedAttributeContainer {
public:
	Attribute(float arc)
		:
		UnsharedAttributeContainer{attr_type::projectile},
		projectile_arc{arc},
		launched{false} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::projectile>>(*this);
	}

	float projectile_arc;
	UnitReference launcher;
	bool launched;
};

/**
 * TODO revisit after unit training is improved
 */
template<> class Attribute<attr_type::building>: public UnsharedAttributeContainer {
public:
	Attribute(int foundation_terrain, UnitType *pp, coord::phys3 gather_point)
		:
		UnsharedAttributeContainer{attr_type::building},
		completed{.0f},
		foundation_terrain{foundation_terrain},
		pp{pp},
		gather_point{gather_point} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::building>>(*this);
	}

	float completed;
	int foundation_terrain;

	// set the TerrainObject to this state
	// once building has been completed
	object_state completion_state;

	// TODO: list allowed trainable producers
	UnitType *pp;

	/**
	 * The go to point after a unit is created.
	 */
	coord::phys3 gather_point;
};

/**
 * The resources that are accepted to be dropped.
 */
template<> class Attribute<attr_type::dropsite>: public SharedAttributeContainer {
public:
	Attribute(std::vector<game_resource> types)
		:
		SharedAttributeContainer{attr_type::dropsite},
		resource_types{types} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::dropsite>>(*this);
	}

	bool accepting_resource(game_resource res) const;

	std::vector<game_resource> resource_types;
};

/**
 * Resource capacity of a trees, mines, animal, worker etc.
 */
template<> class Attribute<attr_type::resource>: public UnsharedAttributeContainer {
public:
	Attribute()
		:
		Attribute{game_resource::food, 0} {}

	Attribute(game_resource type, double init_amount, double decay=0.0, double gather_rate=1.0)
		:
		UnsharedAttributeContainer{attr_type::resource},
		resource_type{type},
		amount{init_amount},
		decay{decay},
		gather_rate{gather_rate} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::resource>>(*this);
	}

	game_resource resource_type;

	double amount;

	/**
	 * The rate of decay
	 */
	double decay;

	/**
	 * The gather rate multiplier (1.0 is the identity)
	 */
	double gather_rate;

};

/**
 * Resource generator eg. relic.
 * While a unit is idle and contains this attribute, it will generate resources for its owner.
 *
 * A rate of zero means that the generation is continuously and not in intervals.
 */
template<> class Attribute<attr_type::resource_generator>: public SharedAttributeContainer {
public:

	Attribute(ResourceBundle resources, double rate=0)
		:
		SharedAttributeContainer{attr_type::resource_generator},
		resources{resources},
		rate{rate} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::resource_generator>>(*this);
	}

	ResourceBundle resources;

	double rate;

};

/**
 * The worker's capacity and gather rates.
 */
template<> class Attribute<attr_type::worker>: public SharedAttributeContainer {
public:
	Attribute()
		:
		SharedAttributeContainer{attr_type::worker} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::worker>>(*this);
	}

	/**
	 * The max number of resources that can be carried.
	 */
	double capacity;

	/**
	 * The gather rate for each resource.
	 * The ResourceBundle class is used but instead of amounts it stores gather rates.
	 */
	ResourceBundle gather_rate;
};

/**
 * The worker's capacity and gather rates.
 */
template<> class Attribute<attr_type::storage>: public SharedAttributeContainer {
public:
	Attribute()
		:
		SharedAttributeContainer{attr_type::storage} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::storage>>(*this);
	}

	/**
	 * The capacity for each resource.
	 */
	ResourceBundle capacity;
};

class Unit;

/**
 * Stores the collection of unit types based on a unit class.
 * It is used mostly for units with multiple graphics (villagers, trebuchets).
 */
template<> class Attribute<attr_type::multitype>: public SharedAttributeContainer {
public:
	Attribute()
		:
		SharedAttributeContainer{attr_type::multitype} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::multitype>>(*this);
	}

	/**
	 * Switch the type of a unit based on a given unit class
	 */
	void switchType(const gamedata::unit_classes cls, Unit *unit) const;

	/**
	 * The collection of unit class to unit type pairs
	 */
	std::unordered_map<gamedata::unit_classes, UnitType *> types;
};

/**
 * Units put inside a building.
 * TODO add capacity per type of unit
 */
template<> class Attribute<attr_type::garrison>: public UnsharedAttributeContainer {
public:
	Attribute()
		:
		UnsharedAttributeContainer{attr_type::garrison} {}

	std::shared_ptr<AttributeContainer> copy() const override {
		return std::make_shared<Attribute<attr_type::garrison>>(*this);
	}

	/**
	 * The units that are garrisoned.
	 */
	std::vector<UnitReference> content;
};

} // namespace openage
