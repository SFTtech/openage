// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GAME_MAIN_H_
#define OPENAGE_GAME_MAIN_H_

#include <SDL2/SDL.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "args.h"
#include "assetmanager.h"
#include "datamanager.h"
#include "engine.h"
#include "coord/tile.h"
#include "handlers.h"
#include "terrain/terrain.h"
#include "terrain/terrain_object.h"
#include "unit/selection.h"
#include "unit/unit_container.h"
#include "util/externalprofiler.h"
#include "gamedata/gamedata.gen.h"

namespace openage {

class Unit;
class UnitProducer;

/**
 * runs the game.
 */
int run_game(openage::Arguments *args);

void gametest_init(openage::Engine *engine);
void gametest_destroy();

class GameMain :
		openage::InputHandler,
		openage::DrawHandler,
		openage::HudHandler,
		openage::TickHandler {
public:
	GameMain(openage::Engine *engine);
	~GameMain();

	void move_camera();

	virtual bool on_tick();
	virtual bool on_draw();
	virtual bool on_drawhud();
	virtual bool on_input(SDL_Event *e);

	/**
	 * all the buildings that have been placed.
	 */
	UnitContainer placed_units;

	/**
	 * debug function that draws a simple overlay grid
	 */
	void draw_debug_grid();

	// currently selected terrain id
	openage::terrain_t editor_current_terrain;
	int editor_current_building;

	bool debug_grid_active;
	bool clicking_active;
	bool ctrl_active;
	bool scrolling_active;
	bool draging_active;
	bool construct_mode;
	bool building_placement;

	// mouse position
	coord::camgame mousepos_camgame;
	coord::phys3 mousepos_phys3;
	coord::tile mousepos_tile;

	UnitSelection selection;
	Terrain *terrain;
	Texture *gaben;

	AssetManager assetmanager;
	DataManager datamanager;

	util::ExternalProfiler external_profiler;
private:

	/**
	 * decides which type of right nouse click command to issue based on position
	 *
	 * if a unit is at the position the command should target the unit,
	 * otherwise target ground position
	 */
	Command get_action(const coord::phys3 &pos) const;

	openage::Engine *engine;
};

} //namespace openage

#endif
