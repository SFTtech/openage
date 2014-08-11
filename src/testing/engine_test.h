#ifndef _OPENAGE_INIT_H_
#define _OPENAGE_INIT_H_

#include <SDL2/SDL.h>
#include <unordered_set>

#include "../engine.h"
#include "../handlers.h"
#include "../terrain.h"


namespace testing {

/**
 * run the main testing routine for the game.
 *
 * this invokes the main game for now.
 */
int run_game(openage::Arguments *args);

void gametest_init(openage::Engine *engine);
void gametest_destroy();

bool on_engine_tick();
bool draw_method();
bool hud_draw_method();
bool input_handler(SDL_Event *e);

class EngineTest :
		openage::InputHandler,
		openage::DrawHandler,
		openage::HudHandler,
		openage::TickHandler {
public:
	EngineTest(openage::Engine *engine);
	~EngineTest();

	void move_camera();

	virtual bool on_tick();
	virtual bool on_draw();
	virtual bool on_drawhud();
	virtual bool on_input(SDL_Event *e);

	//all the buildings that have been placed
	std::unordered_set<openage::TerrainObject *> buildings;

	//currently selected terrain id
	openage::terrain_t editor_current_terrain;

	bool clicking_active;
	bool scrolling_active;

	openage::Terrain *terrain;
	openage::Texture *gaben, *university;
	openage::audio::Sound *build_uni_sound, *destroy_uni_sound0, *destroy_uni_sound1;
};

} //namespace testing

#endif //_OPENAGE_INIT_H_
