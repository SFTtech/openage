// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <queue>

#include "../log/logsource.h"
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
class Unit : public log::LogSource {
public:
	Unit(UnitContainer &c, id_t id);

	/**
	 * unit cleanup will delete terrain object
	*/
	virtual ~Unit();

	/**
	 * this units unique id value
	 */
	const id_t id;

	/**
	 * type of this object, this is set by the the UnitType which
	 * was most recently applied to this unit
	 */
	const UnitType *unit_type;

	/**
	 * should selection features be drawn
	 * TODO: should be a pointer to selection to be updated
	 * when unit is removed, or null if not selected
	 */
	bool selected;

	/**
	 * space on the map used by this unit
	 * null if the object is not yet placed or garrisoned
	 * TODO: make private field
	 */
	std::unique_ptr<TerrainObject> location;

	/**
	 * constructs a new location for this unit replacing any
	 * existing location
	 *
	 * uses same args as the location constructor
	 * except the first which will filled automatically
	 */
	template<class T, typename ... Arg>
	void make_location(Arg ... args) {

		// remove any existing location first
		if (this->location) {
			this->location->remove();
		}

		// since Unit is a friend of the location
		// make_shared will not work
		this->location = std::unique_ptr<T>(new T(*this, args ...));
	}

	/**
	 * removes all actions and abilities
	 * current attributes are kept
	 */
	void reset();

	/**
	 * checks the entity has an action, if it has no action it should be removed from the game
	 * @return true if the entity currently has an action
	 */
	bool has_action() const;

	/**
	 * true when the unit is alive and able to add new actions
	 */
	bool accept_commands() const;

	/**
	 * checks whether the current player is the owner of this unit
	 */
	bool is_own_unit();

	/**
	 * returns the current action on top of the stack
	 */
	UnitAction *top() const;

	/**
	 * returns action under the passed action in the stack
	 * returns null if stack size is less than 2
	 */
	UnitAction *before(const UnitAction *action) const;

	/**
	 * update this object using the action currently on top of the stack
	 */
	bool update();

	/**
	 * draws this action by taking the graphic type of the top action
	 * the graphic is found from the current graphic set
	 *
	 * this function should be used for most draw purposes
	 */
	void draw();

	/**
	 * an generalized draw function which is useful for drawing annexes
	 */
	void draw(TerrainObject *loc, const graphic_set &graphics);

	/**
	 * draws with a specific graphic and frame
	 */
	void draw(coord::phys3 draw_pos, std::shared_ptr<UnitTexture> graphic, unsigned int frame);

	/**
	 * adds an available ability to this unit
	 * this turns targeted objects into actions which are pushed
	 * onto the stack, eg. targeting a relic may push a collect relic action
	 */
	void give_ability(std::shared_ptr<UnitAbility>);

	/**
	 * get ability with specified type, null if not available
	 *
	 * To invoke commands use the invoke function instead
	 */
	UnitAbility *get_ability(ability_type type);

	/**
	 * adds a new action on top of the action stack
	 * will be performed immediately
	 */
	void push_action(std::unique_ptr<UnitAction> action, bool force=false);

	/**
	 * adds a seconadry action which is always updated
	 */
	void secondary_action(std::unique_ptr<UnitAction> action);

	/**
	 * give a new attribute this this unit
	 * this is used to set things like color, hitpoints and speed
	 */
	void add_attribute(std::shared_ptr<AttributeContainer> attr);

	/**
	 * returns whether attribute is available
	 */
	bool has_attribute(attr_type type) const;

	/**
	 * returns attribute based on templated value
	 */
	template<attr_type T>
	Attribute<T> &get_attribute() {
		return *reinterpret_cast<Attribute<T> *>(attribute_map[T].get());
	}

	/**
	 * applies the command to this unit
	 *
	 * a direct command discards all interruptible tasks and sets a new target
	 * for this entity to complete, and play action sound if available
	 *
	 * @return true if an action was created
	 */
	bool queue_cmd(const Command &cmd);

	/**
	 * removes all gather actions without calling their on_complete actions
	 * this cancels the gathering action completely
	 */
	void stop_gather();

	/**
	 * removes all actions above and including the first interuptable action
	 * this will stop any of the units current moving or attacking actions
	 */
	void stop_actions();

	/**
	 * begins unit removal by popping some actions
	 *
	 * this is the action that occurs when pressing the delete key
	 * which plays death sequence and does not remove instantly
	 */
	void delete_unit();

	/**
	 * get a reference which can check against the container
	 * to ensure this object still exists
	 */
	UnitReference get_ref();

	/**
	 * the container used when constructing this unit
	 */
	UnitContainer *get_container() const;

	/**
	 *
	 */
	std::vector<UnitAction *> current_actions() const;

	/**
	 * Returns the unit's name as the LogSource name.
	 */
	std::string logsource_name() override;

private:
	/**
	 * ability available -- actions that this entity
	 * can perform when controlled
	 */
	std::unordered_map<ability_type, std::shared_ptr<UnitAbility>> ability_available;


	/**
	 * action stack -- top action determines graphic to be drawn
	 */
	std::vector<std::unique_ptr<UnitAction>> action_stack;


	/**
	 * seconadry actions are always updated
	 */
	std::vector<std::unique_ptr<UnitAction>> action_secondary;


	/**
	 * queue commands to be applied on the next update
	 */
	std::queue<std::pair<std::shared_ptr<UnitAbility>, const Command>> command_queue;

	/**
	 * mutex controlling updates to the command queue
	 */
	std::mutex command_queue_lock;


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
	UnitContainer &container;

	/**
	 * applies new commands as part of the units update process
	 */
	void apply_all_cmds();

	/**
	 * applies one command using a chosen ability
	 * locks the command queue mutex while operating
	 */
	void apply_cmd(std::shared_ptr<UnitAbility> ability, const Command &cmd);

	/**
	 * update all secondary actions
	 */
	void update_secondary(int64_t time_elapsed);

	/**
	 * erase from action specified by func to the end of the stack
	 * all actions erased will have the on_complete function called
	 *
	 * @param run_completed usually each action has an on_complete() function called when it is removed
	 * but when run_completed is false this on_complete() function is not called for all popped actions
	 */
	void erase_after(std::function<bool(std::unique_ptr<UnitAction> &)> func, bool run_completed=true);

};

} // namespace openage
