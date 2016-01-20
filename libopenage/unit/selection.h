// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UNIT_SELECTION_H_
#define OPENAGE_UNIT_SELECTION_H_

#include <vector>

#include "../coord/camgame.h"
#include "../handlers.h"
#include "ability.h"
#include "unit_container.h"

namespace openage {

class Terrain;

std::vector<coord::tile> tiles_in_range(coord::camgame p1, coord::camgame p2);

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
	UnitSelection();

	bool on_drawhud() override;
	void drag_begin(coord::camgame pos);
	void drag_update(coord::camgame pos);
	void drag_release(Terrain *terrain, bool append=false);

	void clear();

	void toggle_unit(Unit *u, bool append=false);
	void add_unit(Unit *u, bool append=false);
	void remove_unit(Unit *u);

	/**
	 * kill a single unit in the selection
	 */
	void kill_unit();

	/**
	 * checks whether there are any builders in the selection
	 */
	bool contains_builders();

	/**
	 * point unit selection
	 */
	void select_point(Terrain *terrain, coord::camgame p, bool append=false);

	/**
	 * boxed unit selcetion
	 */
	void select_space(Terrain *terrain, coord::camgame p1, coord::camgame p2, bool append=false);

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
	selection_type_t get_unit_selection_type(Unit *);

	std::unordered_map<id_t, UnitReference> units;
	selection_type_t selection_type;

	bool drag_active;
	coord::camgame start, end;
	int font_size;
};

} // namespace openage

#endif
