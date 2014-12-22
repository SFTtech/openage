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


/**
 * wraps a templated attribute
 */
class AttributeContainer {
public:
	virtual ~AttributeContainer() = 0;
};

inline AttributeContainer::~AttributeContainer(){
}

// -----------------------------
// attribute definitions go here
// -----------------------------

template<attr_type T>
class Attribute;

template<> class Attribute<attr_type::color>: public AttributeContainer {
public:
	Attribute(uint c)
		:
		color{c} {}

	uint color;
};

template<> class Attribute<attr_type::hitpoints>: public AttributeContainer {
public:
	Attribute(uint i, uint m)
		:
		current{i},
		max{m} {}

	uint current;
	uint max;
};

template<> class Attribute<attr_type::speed>: public AttributeContainer {
public:
	Attribute(coord::phys_t sp)
		:
		unit_speed{sp} {}

	coord::phys_t unit_speed; // possibly use a pointer to account for tech upgrades
};

template<> class Attribute<attr_type::direction>: public AttributeContainer {
public:
	Attribute(coord::phys3_delta dir)
		:
		unit_dir(dir) {}

	coord::phys3_delta unit_dir;
};

template<> class Attribute<attr_type::dropsite>: public AttributeContainer {
public:
	uint resource_type; // todo resource type enum
};

} // namespace openage

#endif
