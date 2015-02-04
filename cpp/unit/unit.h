// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_UNIT_H_
#define OPENAGE_UNIT_UNIT_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "../coord/phys3.h"
#include "../handlers.h"
#include "ability.h"
#include "attribute.h"
#include "unit_container.h"

namespace openage {

class TerrainObject;
class UnitAbility;
class UnitAction;

/**
 * A game object with current state represented by a stack of actions
 * since this class represents both unit and building objects it may be better to
 * name as GameObject
 */
class Unit {
public:
	Unit(UnitContainer *c, id_t id);
	virtual ~Unit();

	/**
	 * this units unique id value
	 */
	const id_t id;

	/**
	 * space on the map used by this unit
	 * null if the object is not yet placed or garrisoned
	 */
	TerrainObject *location;

	/**
	 * checks the entity has an action, if it has no action it should be removed from the game
	 * @return true if the entity currently has an action
	 */
	bool has_action();

	/**
	 * update this object using the action currently on top of the stack
	 */
	bool update();

	/**
	 * draw this object using the action currently on top of the stack
	 */
	bool draw();

	/**
	 * adds an available ability to this unit
	 * this turns targeted objects into actions which are pushed
	 * onto the stack, eg. targeting a relic may push a collect relic action
	 */
	void give_ability(std::unique_ptr<UnitAbility>);

	/**
	 * get ability with specified type, null if not available
	 */
	UnitAbility *get_ability(ability_type type);

	/**
	 * adds a new action on top of the action stack
	 * will be performed immediately
	 */
	void push_action(std::unique_ptr<UnitAction>);

	/**
	 * give a new attribute this this unit
	 * this is used to set things like color, hitpoints and speed
	 */
	void add_attribute(AttributeContainer *attr);

	/**
	 * returns whether attribute is available
	 */
	bool has_attribute(attr_type type);

	/**
	 * returns attribute based on templated value
	 */
	template<attr_type T> Attribute<T> &get_attribute() {
		//return attribute_map[T]->get<T>();
	 return *reinterpret_cast<Attribute<T> *>(attribute_map[T]);
	}

	/**
	 * discards all interruptible tasks and sets a new target
	 * for this entity to move towards
	 *
	 * @param type allows a specific ability type to be used
	 * this is used to set a unit to patrol rather than the default move
	 * @return true if an action was created
	 */
	bool target(coord::phys3 target, ability_set type=ability_all);
	bool target(Unit *target,        ability_set type=ability_all);

	/**
	 * delete action
	 */
	void delete_unit();

	/**
	 * get a reference which can check against the container
	 * to ensure this object still exists
	 */
	UnitReference get_ref();

private:
	/**
	 * ability available -- actions that this entity
	 * can perform when controlled
	 */
	std::vector<std::unique_ptr<UnitAbility>> ability_available;

	/**
	 * action stack -- top action determines graphic to be drawn
	 */
	std::vector<std::unique_ptr<UnitAction>> action_stack;

	/**
	 * Unit attributes include color, hitpoints, speed, objects garrisoned etc
	 * contains 0 or 1 values for each type
	 */
	attr_map_t attribute_map;

	/**
	 * pop any destructable actions on the next update cycle
	 */
	bool pop_destructables;

	/**
	 * the container that updates this unit
	 */
	const UnitContainer *container;

	/**
	 * removes all actions above and including the first interuptable action
	 * this will stop any of the units current moving or attacking actions
	 */
	void erase_interuptables();
};

/**
 * the set of images to used based on unit direction,
 * usually 8 directions to draw for each unit (3 are mirrored)
 *
 * @param dir a world space direction,
 * @param angles number of angles, usually 8
 * @param first_angle offset added to angle, modulo number of angles
 * @return image set index
 */
unsigned int dir_group(coord::phys3_delta dir, unsigned int angles=8, unsigned int first_angle=5);

} // namespace openage

#endif
