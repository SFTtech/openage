// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_ATTRIBUTE_H_
#define OPENAGE_UNIT_ATTRIBUTE_H_

#include <functional>
#include <map>

#include "../coord/tile.h"
#include "../gamedata/unit.gen.h"
#include "resource.h"
#include "unit_container.h"

namespace std {

/**
 * hasher for unit classes enum type
 */
template<> struct hash<gamedata::unit_classes> {
	typedef underlying_type<gamedata::unit_classes>::type underlying_type;
	typedef hash<underlying_type>::result_type result_type;
	result_type operator()(const gamedata::unit_classes &arg) const {
		hash<underlying_type> hasher;
		return hasher(static_cast<underlying_type>(arg));
	}
};

} // namespace std

namespace openage {

/**
 * types of action graphics
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
	work
};

class UnitTexture;

/**
 * collection of graphics attached to each unit
 */
using graphic_set = std::map<graphic_type, std::shared_ptr<UnitTexture>>;

/**
 * list of attribute types
 */
enum class attr_type {
	owner,
	hitpoints,
	attack,
	speed,
	direction,
	projectile,
	building,
	resource,
	gatherer,
	garrison
};

enum class attack_stance {
	aggresive,
	devensive,
	stand_ground,
	do_nothing
};

/**
 * this type gets specialized for each attribute
 */
template<attr_type T> class Attribute;

/**
 * wraps a templated attribute
 */
class AttributeContainer {
public:
	AttributeContainer() {}

	AttributeContainer(attr_type t)
		:
		type{t} {}

	attr_type type;

	virtual AttributeContainer *copy() const = 0;
};

using attr_map_t = std::map<attr_type, AttributeContainer *>;


/**
 * return attribute from a container
 */
template<attr_type T> Attribute<T> get_attr(attr_map_t &map) {
	return *reinterpret_cast<Attribute<T> *>(map[T]);
}

// -----------------------------
// attribute definitions go here
// -----------------------------

class Player;

template<> class Attribute<attr_type::owner>: public AttributeContainer {
public:
	Attribute(Player &p)
		:
		AttributeContainer{attr_type::owner},
		player(p) {}

	AttributeContainer *copy() const override {
		return new Attribute<attr_type::owner>(*this);
	}

	Player &player;
};

template<> class Attribute<attr_type::hitpoints>: public AttributeContainer {
public:
	Attribute(unsigned int i)
		:
		AttributeContainer{attr_type::hitpoints},
		current{static_cast<int>(i)},
		max{i} {}

	AttributeContainer *copy() const override {
		return new Attribute<attr_type::hitpoints>(*this);
	}

	int current; // can become negative
	unsigned int max;
	float hp_bar_height;
};

template<> class Attribute<attr_type::attack>: public AttributeContainer {
public:
	Attribute(UnitType *type, coord::phys_t r, coord::phys_t h, uint d, graphic_set &grp)
		:
		AttributeContainer{attr_type::attack},
		ptype{type},
		range{r},
		init_height{h},
		damage{d},
		stance{attack_stance::do_nothing},
		attack_graphic_set{&grp} {}

	AttributeContainer *copy() const override {
		return new Attribute<attr_type::attack>(*this);
	}

	// TODO: can a unit have multiple attacks such as villagers hunting
	// map target classes onto attacks

	UnitType *ptype; // projectile type
	coord::phys_t range;
	coord::phys_t init_height;
	unsigned int damage;
	attack_stance stance;

	// used to change graphics back to normal for villagers
	graphic_set *attack_graphic_set;
};

template<> class Attribute<attr_type::speed>: public AttributeContainer {
public:
	Attribute(coord::phys_t sp)
		:
		AttributeContainer{attr_type::speed},
		unit_speed{sp} {}

	AttributeContainer *copy() const override {
		return new Attribute<attr_type::speed>(*this);
	}

	coord::phys_t unit_speed; // possibly use a pointer to account for tech upgrades
};

template<> class Attribute<attr_type::direction>: public AttributeContainer {
public:
	Attribute(coord::phys3_delta dir)
		:
		AttributeContainer{attr_type::direction},
		unit_dir(dir) {}

	AttributeContainer *copy() const override {
		return new Attribute<attr_type::direction>(*this);
	}

	coord::phys3_delta unit_dir;
};

template<> class Attribute<attr_type::projectile>: public AttributeContainer {
public:
	Attribute(float arc)
		:
		AttributeContainer{attr_type::projectile},
		projectile_arc{arc},
		launched{false} {}

	AttributeContainer *copy() const override {
		return new Attribute<attr_type::projectile>(*this);
	}

	float projectile_arc;
	UnitReference launcher;
	bool launched;
};

template<> class Attribute<attr_type::building>: public AttributeContainer {
public:
	Attribute()
		:
		AttributeContainer{attr_type::building},
		completed{.0f},
		is_dropsite{true},
		foundation_terrain{0} {}

	AttributeContainer *copy() const override {
		return new Attribute<attr_type::building>(*this);
	}

	float completed;
	bool is_dropsite;
	int foundation_terrain;

	// TODO: use unit class, fish and forage have different dropsites
	game_resource resource_type;

	// TODO: list allowed trainable producers
	UnitType *pp;
	coord::phys3 gather_point;
};

/**
 * resource capacity of an object, trees, mines, villagers etc.
 */
template<> class Attribute<attr_type::resource>: public AttributeContainer {
public:
	Attribute(game_resource type, float init_amount)
		:
		AttributeContainer{attr_type::resource},
		resource_type{type},
		amount{init_amount} {}

	AttributeContainer *copy() const override {
		return new Attribute<attr_type::resource>(*this);
	}

	game_resource resource_type;
	float amount;
};

class UnitTexture;

/**
 * TODO: rename to worker
 */
template<> class Attribute<attr_type::gatherer>: public AttributeContainer {
public:
	Attribute()
		:
		AttributeContainer{attr_type::gatherer},
		amount{.0f} {}

	AttributeContainer *copy() const override {
		return new Attribute<attr_type::gatherer>(*this);
	}

	game_resource current_type;
	float amount;
	float capacity;
	float gather_rate;

	// texture sets available for each resource
	std::unordered_map<gamedata::unit_classes, UnitType *> graphics;
};

template<> class Attribute<attr_type::garrison>: public AttributeContainer {
public:
	Attribute()
		:
		AttributeContainer{attr_type::garrison} {}

	AttributeContainer *copy() const override {
		return new Attribute<attr_type::garrison>(*this);
	}

	std::vector<UnitReference> content;
};

} // namespace openage

#endif
