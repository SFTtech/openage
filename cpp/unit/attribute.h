// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_ATTRIBUTE_H_
#define OPENAGE_UNIT_ATTRIBUTE_H_

#include <map>

#include "../coord/tile.h"

namespace openage {

/**
 * list of attribute types
 */
enum class attr_type {
	color,
	hitpoints,
	speed,
	direction,
	dropsite
};

template<attr_type T> class Attribute;

/**
 * wraps a templated attribute
 */
class AttributeContainer {
public:
	AttributeContainer() {}

	AttributeContainer(attr_type t)
		:
		type(t) {}

	attr_type type;
};

using attr_map_t = std::map<attr_type, AttributeContainer *>;

template<attr_type T> Attribute<T> get_attr(attr_map_t &map) {
	return *reinterpret_cast<Attribute<T> *>(map[T]);
}

// -----------------------------
// attribute definitions go here
// -----------------------------

template<> class Attribute<attr_type::color>: public AttributeContainer {
public:
	Attribute(uint c)
		:
		AttributeContainer(attr_type::color),
		color(c) {}

	uint color;
};

template<> class Attribute<attr_type::hitpoints>: public AttributeContainer {
public:
	Attribute(uint i, uint m)
		:
		AttributeContainer(attr_type::hitpoints),
		current(i),
		max(m) {}

	uint current;
	uint max;
};

template<> class Attribute<attr_type::speed>: public AttributeContainer {
public:
	Attribute(coord::phys_t sp)
		:
		AttributeContainer(attr_type::speed),
		unit_speed(sp) {}

	coord::phys_t unit_speed; // possibly use a pointer to account for tech upgrades
};

template<> class Attribute<attr_type::direction>: public AttributeContainer {
public:
	Attribute(coord::phys3_delta dir)
		:
		AttributeContainer(attr_type::direction),
		unit_dir(dir) {}

	coord::phys3_delta unit_dir;
};

template<> class Attribute<attr_type::dropsite>: public AttributeContainer {
public:
	Attribute()
		:
		AttributeContainer(attr_type::dropsite) {}

	uint resource_type; // todo resource type enum
};

} // namespace openage

#endif
