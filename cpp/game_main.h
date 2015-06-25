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
#include "keybinds/keybind_manager.h"
#include "player.h"
#include "terrain/terrain.h"
#include "terrain/terrain_object.h"
#include "unit/selection.h"
#include "unit/unit_container.h"
#include "util/externalprofiler.h"
#include "gamedata/gamedata.gen.h"
#include "gui/forward.h"

namespace openage {

class Unit;

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

	bool on_tick() override;
	bool on_draw() override;
	bool on_drawhud() override;
	bool on_input(SDL_Event *e) override;

	/**
	 * debug function that draws a simple overlay grid
	 */
	void draw_debug_grid();

	// currently selected terrain id
	openage::terrain_t editor_current_terrain;
	int editor_current_building;

	bool debug_grid_active;
	bool clicking_active;
	bool scrolling_active;
	bool dragging_active;
	bool construct_mode;
	bool building_placement;
	bool use_set_ability;
	ability_type ability;

	// mouse position
	coord::camgame mousepos_camgame;
	coord::phys3 mousepos_phys3;
	coord::tile mousepos_tile;

	UnitSelection selection;
	std::shared_ptr<Terrain> terrain;
	Texture *gaben;
	std::vector<Player> players;

	/**
	 * all the objects that have been placed.
	 */
	UnitContainer placed_units;

	AssetManager assetmanager;
	DataManager datamanager;

	keybinds::KeybindContext keybind_context;

	util::ExternalProfiler external_profiler;
private:

	/**
	 * decides which type of right mouse click command to issue based on position
	 *
	 * if a unit is at the position the command should target the unit,
	 * otherwise target ground position
	 */
	Command get_action(const coord::phys3 &pos) const;

	openage::Engine *engine;

	std::unique_ptr<gui::TopLevel> gui_top_level;
	gui::Label *gui_loading_message;
	gui::Label *gui_framerate;
};

} //namespace openage

#endif
