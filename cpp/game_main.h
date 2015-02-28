// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GAME_MAIN_H_
#define OPENAGE_GAME_MAIN_H_

#include <SDL2/SDL.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "args.h"
#include "assetmanager.h"
#include "engine.h"
#include "coord/tile.h"
#include "handlers.h"
#include "terrain/terrain.h"
#include "terrain/terrain_object.h"
#include "gamedata/graphic.gen.h"
#include "unit/unit_container.h"
#include "util/externalprofiler.h"
#include "gamedata/gamedata.gen.h"
#include "job/job.h"

namespace openage {

class Unit;
class UnitProducer;

/**
 * runs the game.
 */
int run_game(openage::Arguments *args);

void gametest_init(openage::Engine *engine);
void gametest_destroy();

bool on_engine_tick();
bool draw_method();
bool hud_draw_method();
bool input_handler(SDL_Event *e);

class TestBuilding {
public:
	Texture *texture;
	std::string name;
	openage::coord::tile_delta foundation_size;
	int foundation_terrain;
	int sound_id_creation;
	int sound_id_destruction;
};

class TestSound {
public:
	void play();

	std::vector<int> sound_items;
};

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
	 * return a texture handle associated with the given graphic id.
	 */
	Texture *find_graphic(int graphic_id);

	/**
	 * return a testsound ptr for a given sound id.
	 */
	TestSound *find_sound(int sound_id);

	/**
	 * all available game objects.
	 */
	std::vector<std::unique_ptr<UnitProducer>> available_objects;

	/**
	 * all available sounds.
	 */
	std::unordered_map<int, TestSound> available_sounds;

	/**
	 * map graphic id to gamedata graphic.
	 */
	std::unordered_map<int, gamedata::graphic *> graphics;

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
	bool construct_mode;

	Unit *selected_unit;
	Terrain *terrain;
	Texture *gaben;

	AssetManager assetmanager;

	util::ExternalProfiler external_profiler;
private:
	void on_gamedata_loaded(std::vector<gamedata::empiresdat> &gamedata);

	bool gamedata_loaded;

	openage::Engine *engine;
};

} //namespace openage

#endif
