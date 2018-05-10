// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../coord/phys3.h"
#include "attributes.h"

namespace openage {

class Player;
class Terrain;
class TerrainObject;
class Unit;
class UnitAbility;
class UnitContainer;
class UnitTexture;


/**
 * an abstract unit type which is not yet owned by any player
 */
class UnitTypeMeta {
public:
	using type_ptr = std::shared_ptr<UnitType>;
	using init_func = std::function<type_ptr(const Player &owner)>;
	UnitTypeMeta(const std::string &name, int id, init_func f);

	std::string name() const;

	int id() const;

	/**
	 * creates the base unit type for a player
	 */
	const init_func init;

private:
	const std::string type_name;
	const int type_id;

};

/**
 * UnitType has to main roles:
 *
 * initialise(unit, player) should be called on a unit to give it a type and the required attributes, abilities and initial actions
 * of that type
 *
 * place(unit, terrain, initial position) is called to customise how the unit gets added to the world -- used to setup the TerrainObject location
 *
 * UnitType is connected to a player to allow independent tech levels
 */
class UnitType {
public:
	UnitType(const Player &owner);
	virtual ~UnitType() {}

	/**
	 * gets the unique id of this unit type
	 */
	virtual int id() const = 0;

	/**
	 * gets the parent id of this unit type
	 * which is used for village base and gather types
	 */
	virtual int parent_id() const = 0;

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
	 * Initialize units shared attributes only to this type spec
	 *
	 * This can be called using existing units to modify type if the type
	 * Ensure that the unit has been placed before seting the units type
	 *
	 * TODO define if pure vitrual or not / should be in nyan?
	 */
	virtual void reinitialise(Unit *, Player &);

	/**
	 * set unit in place -- return if placement was successful
	 *
	 * This should be used when initially creating a unit or
	 * when a unit is ungarrsioned from a building or object
	 * TODO: make const
	 */
	virtual TerrainObject *place(Unit *, std::shared_ptr<Terrain>, coord::phys3) const = 0;

	/**
	 * compare if two types are the same
	 */
	bool operator==(const UnitType &other) const;
	bool operator!=(const UnitType &other) const;

	/**
	 * Get a default texture for HUD drawing
	 */
	UnitTexture *default_texture();

	/**
	 * similar to place but places adjacent to an existing object
	 */
	TerrainObject *place_beside(Unit *, TerrainObject const *) const;

	/**
	 * copy attributes of this unit type to a new unit instance
	 */
	void copy_attributes(Unit *unit) const;

	/**
	 * upgrades one attribute of this unit type
	 */
	void upgrade(const std::shared_ptr<AttributeContainer> &attr);

	/**
	 * returns type matching parent_id()
	 */
	UnitType *parent_type() const;

	/**
	 * the player who owns this unit type
	 */
	const Player &owner;

	/**
	 * all instances of units made from this unit type
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
	Attributes default_attributes;

	/**
	 * The cost of the unit.
	 */
	ResourceBundle cost;

	/**
	 * The set of graphics used for this type
	 */
	graphic_set graphics;

	/**
	 * the square dimensions of the placement
	 */
	coord::tile_delta foundation_size;

	/**
	 * raw game data class of this unit instance
	 */
	gamedata::unit_classes unit_class;
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
	NyanType(const Player &owner);
	virtual ~NyanType();

	int id() const override;
	int parent_id() const override;
	std::string name() const override;
	void initialise(Unit *, Player &) override;
	TerrainObject *place(Unit *, std::shared_ptr<Terrain>, coord::phys3) const override;

};

} // namespace openage
