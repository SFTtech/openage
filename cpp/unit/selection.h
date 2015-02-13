// Copyright 2015-2015 the openage authors. See copying.md for legal info.

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
 * a user interface component allowing control of a selected group
 */
class UnitSelection: public HudHandler {
public:
	UnitSelection();

	bool on_drawhud();
	void drag_begin(coord::camgame pos);
	void drag_update(coord::camgame pos);
	void drag_release(Terrain *terrain, bool append=false);

	void clear();

	void toggle_unit(Unit *);

	void select_point(Terrain *terrain, coord::camgame p, bool append=false);

	void select_space(Terrain *terrain, coord::camgame p1, coord::camgame p2, bool append=false);

	/**
	 * TODO: use this function instead
	 */
	void all_invoke(Command &cmd);


	// old and unused functions
	void set_ability(ability_type ability);
	void all_invoke(ability_type ability, uint arg);
	void all_target(Terrain *terrain, coord::phys3 target);



private:
	std::unordered_map<id_t, UnitReference> units;

	/**
	 * TODO: moving this to command
	 */
	ability_set modifiers;

	bool drag_active;
	coord::camgame start, end;

};

} // namespace openage

#endif
