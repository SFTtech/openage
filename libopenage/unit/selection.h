// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>

#include "../coord/pixel.h"
#include "../handlers.h"
#include "ability.h"
#include "unit_container.h"

namespace openage {

class Engine;
class Terrain;

std::vector<coord::tile> tiles_in_range(coord::camgame p1, coord::camgame p2, const coord::CoordManager &coord);

/**
 * A selection of units always has a type
 * You can't select units from multiple types at once
 * Earlier types have precedence over later types
 *
 * So you can select a group of units, or a building (or multiple if append is on)
 * Enemy units, enemy buildings and other objects may only be selected one at a time
 */
enum class selection_type_t {
	own_units,
	own_buildings,
	enemy_unit,
	enemy_building,
	nothing
};

/**
 * a user interface component allowing control of a selected group
 */
class UnitSelection: public HudHandler {
public:
	UnitSelection(Engine *engine);

	bool on_drawhud() override;
	void drag_begin(coord::camgame pos);
	void drag_update(coord::camgame pos);
	void drag_release(const Player &player, Terrain *terrain, bool append=false);

	void clear();

	void toggle_unit(const Player &player, Unit *u, bool append=false);
	void add_unit(const Player &player, Unit *u, bool append=false);
	void remove_unit(Unit *u);

	selection_type_t get_selection_type();

	/**
	 * kill a single unit in the selection
	 */
	void kill_unit(const Player &player);

	/**
	 * checks whether there are any builders in the selection
	 */
	bool contains_builders(const Player &player);

	/**
	 * checks whether there are any military units (i.e. non-builders) in the selection
	 */
	bool contains_military(const Player &player);

	/**
	 * point unit selection
	 */
	void select_point(const Player &player, Terrain *terrain, coord::camgame p, bool append=false);

	/**
	 * boxed unit selection
	 */
	void select_space(const Player &player, Terrain *terrain, coord::camgame p1, coord::camgame p2, bool append=false);

	/**
	 * uses command on every selected unit
	 */
	void all_invoke(Command &cmd);

private:
	/**
	 * Must be given a valid unit to display text attribute indicators.
	 */
	void show_attributes(Unit *);

	/**
	 * Check whether the currently selected units may be selected at the same time
	 * If not, deselect some units
	 * This is the order in which the checks occur:
	 * Own units > own building(s) > enemy unit > enemy building > any object
	 *
	 * So you can select a group of units, or a building (or multiple if append is on)
	 * Enemy units, enemy buildings and other objects may only be selected one at a time
	 */
	selection_type_t get_unit_selection_type(const Player &player, Unit *);

	std::unordered_map<id_t, UnitReference> units;
	selection_type_t selection_type;

	bool drag_active;
	// TODO: turn these into a C++17 optional
	coord::camgame start = {0, 0}, end = {0, 0};
	int font_size;

	/**
	 * Engine where this selection is attached to.
	 */
	Engine *engine;
};

} // namespace openage
