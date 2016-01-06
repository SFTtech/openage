// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_TYPE_PAIR_H_
#define OPENAGE_UNIT_TYPE_PAIR_H_

namespace openage {

/**
 * units in aoc have a class and a type
 */
class UnitType {
public:
	UnitType();

	/**
	 * the unit must have either same class or id as this
	 */
	bool match(class Unit *);

private:
	unsigned int class_id;
	unsigned int unit_type_id;
};

/**
 * many effects in aoc use a pair structure
 * such as attack bonuses, armour and selection commands
 */
class TypePair {
public:
	TypePair();

private:
	UnitType a, b;

};

} // namespace openage

#endif
