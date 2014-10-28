#ifndef OPENAGE_GAME_MAIN_H_4194D00CB964499C959C031D4E836672
#define OPENAGE_GAME_MAIN_H_4194D00CB964499C959C031D4E836672

#include <SDL2/SDL.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "args.h"
#include "engine.h"
#include "coord/tile.h"
#include "handlers.h"
#include "terrain.h"
#include "terrain_object.h"
#include "gamedata/graphic.gen.h"


namespace openage {


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
	openage::Texture *texture;
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
	 * all available buildings.
	 */
	std::vector<TestBuilding *> available_buildings;

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
	std::unordered_set<openage::TerrainObject *> placed_buildings;

	// currently selected terrain id
	openage::terrain_t editor_current_terrain;
	int editor_current_building;

	bool clicking_active;
	bool ctrl_active;
	bool scrolling_active;

	Terrain *terrain;
	Texture *gaben;
};

} //namespace openage

#endif //_GAME_MAIN_H_
