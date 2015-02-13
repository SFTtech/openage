// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_UNIT_H_
#define OPENAGE_UNIT_UNIT_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "../coord/phys3.h"
#include "../terrain/terrain_object.h"
#include "../handlers.h"
#include "ability.h"
#include "attribute.h"
#include "unit_container.h"

namespace openage {

class UnitAbility;
class UnitAction;

/**
 * A game object with current state represented by a stack of actions
 * since this class represents both unit and building objects it may be better to
 * name as GameObject
 *
 * it is possible that abilities are not required here and they could be moved
 * to selection controller -- units only need the attributes
 */
class Unit {
public:
	Unit(UnitContainer *c, id_t id);

	/**
     * unit cleanup will delete terrain object
     */
	virtual ~Unit();

	/**
	 * this units unique id value
	 */
	const id_t id;

	/**
	 * class of this unit instance
	 */
	gamedata::unit_classes unit_class;

	/**
	 * should selection features be drawn
	 * TODO: should be a pointer to selection to be updated
	 * when unit is removed, or null if not selected
	 */	
	bool selected;

	/**
	 * space on the map used by this unit
	 * null if the object is not yet placed or garrisoned
	 */
	TerrainObject *location;

	/**
	 * graphics sets which can be modified for gathering
	 */
	graphic_set *graphics;

	/** 
	 * removes all actions and abilities
	 */
	void clear_actions();

	/**
	 * checks the entity has an action, if it has no action it should be removed from the game
	 * @return true if the entity currently has an action
	 */
	bool has_action();

	/**
	 * returns the current action on top of the stack
	 */
	UnitAction *top();

	/**
	 * update this object using the action currently on top of the stack
	 */
	bool update();

	/**
	 * draws this action by taking the graphic type of the top action
	 * the graphic is found from the current graphic set
	 */
	void draw();

	/**
	 * adds an available ability to this unit
	 * this turns targeted objects into actions which are pushed
	 * onto the stack, eg. targeting a relic may push a collect relic action
	 */
	void give_ability(std::shared_ptr<UnitAbility>);

	/**
	 * get ability with specified type, null if not available
	 */
	UnitAbility *get_ability(ability_type type);

	/**
	 * adds a new action on top of the action stack
	 * will be performed immediately
	 */
	void push_action(std::unique_ptr<UnitAction>, bool force=false);

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

	// these functions will stack actions (do not erase current action)
	bool invoke(ability_type type, uint arg,            bool sound=false);
	bool invoke(ability_type type, coord::phys3 target, bool sound=false);
	bool invoke(ability_type type, Unit *target,        bool sound=false);

	/**
	 * delete action
	 */
	void delete_unit();

	/**
	 * get a reference which can check against the container
	 * to ensure this object still exists
	 */
	UnitReference get_ref();

	UnitContainer *get_container();

private:
	/**
	 * ability available -- actions that this entity
	 * can perform when controlled
	 */
	std::unordered_map<ability_type, std::shared_ptr<UnitAbility>, ability_hash> ability_available;

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
	 * and prevent additional actions being added
	 */
	bool pop_destructables;

	/**
	 * the container that updates this unit
	 */
	UnitContainer *const container;

	/**
	 * removes all actions above and including the first interuptable action
	 * this will stop any of the units current moving or attacking actions
	 */
	void erase_interuptables();
};

} // namespace openage

#endif
