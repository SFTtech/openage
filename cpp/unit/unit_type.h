// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_UNIT_TYPE_H_
#define OPENAGE_UNIT_UNIT_TYPE_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "../coord/phys3.h"
#include "attribute.h"

namespace openage {

class Player;
class Terrain;
class TerrainObject;
class Unit;
class UnitAbility;
class UnitContainer;
class UnitReference;
class UnitTexture;

/**
 * Initializes a unit with the required attributes, each unit type should implement these funcrtions
 * initialise should be called on construction of units 'new Unit(some_unit_producer)'
 * place is called to customise how the unit gets added to the world -- used to setup the TerrainObject position
 */
class UnitType {
public:
	virtual ~UnitType() {}

	/**
	 * gets the unique id of this unit type
	 */
	virtual int id() const = 0;

	/**
	 * gets the name of the unit type being produced
	 */
	virtual std::string name() const = 0;

	/**
	 * Initialize units attributes to this type spec
	 *
	 * This can be called using existing units to modify type
	 * Ensure that the unit has been placed before seting the units type
	 *
	 * TODO: make const
	 */
	virtual void initialise(Unit *, Player &) = 0;

	/**
	 * set unit in place -- return if placement was successful
	 *
	 * This should be used when initially creating a unit or
	 * when a unit is ungarrsioned from a building or object
	 * TODO: make const
	 */
	virtual std::shared_ptr<TerrainObject> place(Unit *, std::shared_ptr<Terrain>, coord::phys3) const = 0;

	/**
	 * Get a default texture for HUD drawing
	 */
	UnitTexture *default_texture();

	/**
	 * similiar to place but places adjacent to an existing object
	 */
	std::shared_ptr<TerrainObject> place_beside(Unit *, std::shared_ptr<TerrainObject>) const;

	/**
	 * all instances of units made from this producer
	 * this could allow all units of a type to be upgraded
	 */
	std::vector<UnitReference> instances;

	/**
	 * abilities given to all instances
	 */
	std::vector<std::shared_ptr<UnitAbility>> type_abilities;

	/**
	 * default attributes which get copied to new units
	 */
	std::vector<std::shared_ptr<AttributeContainer>> default_attributes;

	/**
	 * The set of graphics used for this type
	 */
	graphic_set graphics;
};

/**
 * An example of how nyan can work with the type system
 */
class NyanType: public UnitType {
public:
	/**
	 * TODO: give the parsed nyan attributes
	 * to the constructor 
	 */
	NyanType();
	virtual ~NyanType();

	int id() const override;
	std::string name() const override;
	void initialise(Unit *, Player &) override;
	std::shared_ptr<TerrainObject> place(Unit *, std::shared_ptr<Terrain>, coord::phys3) const override;

};

} // namespace openage

#endif
