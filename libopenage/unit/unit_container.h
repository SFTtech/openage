// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "../coord/tile.h"
#include "../handlers.h"
#include "../util/timing.h"


namespace openage {

class Command;
class Player;
class Terrain;
class TerrainObject;
class Unit;
class UnitContainer;
class UnitType;


/**
 * Type used to identify each single unit in the game.
 */
using id_t = unsigned long int;


/**
 * immutable reference data
 */
struct reference_data {
	reference_data(const UnitContainer *c, id_t id, Unit *);

	const UnitContainer *const container;
	const id_t unit_id;
	Unit *const unit_ptr;
};

/**
 * Reference to a single unit, which may have been removed
 * from the game, check is_valid() before calling get()
 */
class UnitReference {
public:

	/**
	 * create an invalid reference
	 */
	UnitReference();

	/**
	 * create referece by unit id
	 */
	UnitReference(const UnitContainer *c, id_t id, Unit *);

	bool is_valid() const;
	Unit *get() const;

private:

	/**
	 * The default copy constructor and assignment
	 * will just copy the shared pointer
	 */
	std::shared_ptr<reference_data> data;
};

/**
 * the list of units that are currently in use
 * will also give a view of the current game state for networking in later milestones
 */
class UnitContainer {
public:
	UnitContainer();
	~UnitContainer();

	void reset();

	/**
	 * sets terrain to initialise units on
	 */
	void set_terrain(std::shared_ptr<Terrain> &t);

	/**
	 * returns the terrain which units are placed on
	 */
	std::shared_ptr<Terrain> get_terrain() const;

	/**
	 * checks the id is valid
	 */
	bool valid_id(id_t id) const;

	/**
	 * returns a reference to a unit
	 */
	UnitReference get_unit(id_t id);

	/**
	 * creates a new unit without initialising
	 */
	UnitReference new_unit();

	/**
	 * adds a new unit to the container and initialises using a unit type
	 */
	UnitReference new_unit(UnitType &type, Player &owner, coord::phys3 position);

	/**
	 * adds a new unit to the container and initialises using a unit type
	 * places outside an existing object using the player of that object
	 */
	UnitReference new_unit(UnitType &type, Player &owner, TerrainObject *other);

	/**
	 * give a command to a unit -- unit creation and deletion should be done as commands
	 */
	bool dispatch_command(id_t to_id, const Command &cmd);

	/**
	 * update dispatched by the game engine on each physics tick.
	 * this will update all game objects.
	 */
	bool update_all(time_nsec_t lastframe_duration);

	/**
	 * gets a list of all units in the container
	 */
	std::vector<openage::Unit *> all_units();

private:
	id_t next_new_id;

	/**
	 * mapping unit ids to unit objects
	 */
	std::unordered_map<id_t, std::unique_ptr<Unit>> live_units;

	/**
	 * Terrain for initialising new units
	 */
	std::weak_ptr<Terrain> terrain;
};

} // namespace openage
