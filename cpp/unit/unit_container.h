// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_UNIT_CONTAINER_H_
#define OPENAGE_UNIT_UNIT_CONTAINER_H_

#include <memory>
#include <unordered_map>

#include "../coord/tile.h"
#include "../handlers.h"

namespace openage {

class Command;
class Terrain;
class Unit;
class UnitProducer;

/**
 * the list of units that are currently in use
 * will also give a view of the current game state for networking in later milestones
 */
class UnitContainer : public TickHandler {
public:
	UnitContainer();
	~UnitContainer();

	/**
	 * adds a new unit to the container
	 */
	bool new_unit(std::shared_ptr<UnitProducer> producer, Terrain *terrain, coord::tile tile);

	/**
	 * give a command to a unit -- unit creation and deletion should be done as commands
	 */
	bool dispatch_command(uint to_id, const Command &cmd);

	/**
	 * update dispatched by the game engine
	 * this will update all game objects
	 */
	bool on_tick();

private:
	uint next_new_id;

	/**
	 * mapping unit ids to unit objects
	 */
	std::unordered_map<uint, Unit *> live_units;

};

} // namespace openage

#endif
