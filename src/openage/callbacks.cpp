#include "callbacks.h"

#include <stdlib.h>

#include "gamestate.h"
#include "../engine/engine.h"
#include "../engine/log.h"
#include "../engine/coord/window.h"
#include "../engine/coord/camgame.h"
#include "../engine/coord/camhud.h"
#include "../engine/coord/phys3.h"
#include "../engine/coord/tile.h"
#include "../engine/coord/tile3.h"
#include "../engine/coord/vec2f.h"
#include "../engine/util/misc.h"
#include "../engine/input.h"


using namespace engine;

namespace openage {

bool clicking_active = true;
bool scrolling_active = false;

bool input_handler(SDL_Event *e) {
	switch (e->type) {

	case SDL_QUIT:
		engine::running = false;
		break;

	case SDL_MOUSEBUTTONDOWN: { //thanks C++! we need a separate scope because of new variables...

		//a mouse button was pressed...
		//subtract value from window height to get position relative to lower right (0,0).
		coord::window mousepos_window {(coord::pixel_t) e->button.x, (coord::pixel_t) e->button.y};
		coord::camgame mousepos_camgame = mousepos_window.to_camgame();
		//TODO once the terrain elevation milestone is implemented, use a method
		//more suitable for converting camgame to phys3
		coord::phys3 mousepos_phys3 = mousepos_camgame.to_phys3();
		coord::tile mousepos_tile = mousepos_phys3.to_tile3().to_tile();

		if (clicking_active and e->button.button == SDL_BUTTON_LEFT) {
			log::dbg("LMB [window]:    x %9hd y %9hd",
			         mousepos_window.x,
			         mousepos_window.y);
			log::dbg("LMB [camgame]:   x %9hd y %9hd",
			         mousepos_camgame.x,
			         mousepos_camgame.y);
			log::dbg("LMB [phys3]:     NE %8.3f SE %8.3f UP %8.3f",
			         ((float) mousepos_phys3.ne) / coord::phys_per_tile,
			         ((float) mousepos_phys3.se) / coord::phys_per_tile,
			         ((float) mousepos_phys3.up) / coord::phys_per_tile);
			log::dbg("LMB [tile]:      NE %8ld SE %8ld",
			         mousepos_tile.ne,
			         mousepos_tile.se);

			TerrainChunk *chunk = terrain->get_create_chunk(mousepos_tile);
			chunk->get_data(mousepos_tile)->terrain_id = editor_current_terrain;
		}
		else if (clicking_active and e->button.button == SDL_BUTTON_RIGHT) {

			//get chunk clicked on, don't create it if it's not there already
			TerrainChunk *chunk = terrain->get_chunk(mousepos_tile);
			if (chunk == nullptr) {
				break;
			}

			//get object currently standing at the clicked position
			TerrainObject *obj = chunk->get_data(mousepos_tile)->obj;
			if (obj != nullptr) {
				obj->remove();
				buildings.erase(obj);
				delete obj;

				//play uni destruction sound
				int rand = util::random_range(0, 1 + 1);
				if (rand == 0) {
					destroy_uni_sound0->play();
				} else {
					destroy_uni_sound1->play();
				}
			} else {
				int uni_player_id = util::random_range(1, 8 + 1);
				TerrainObject *newuni = new TerrainObject(university, uni_player_id);

				log::dbg("uni player id: %d", uni_player_id);

				//try to place the uni, it knows best whether it will fit.
				bool uni_placed = newuni->place(terrain, mousepos_tile);
				if (uni_placed) {
					newuni->set_ground(editor_current_terrain, 0);
					buildings.insert(newuni);
					build_uni_sound->play();
				} else {
					delete newuni;
				}
			}
			break;
		}
		else if (not scrolling_active and e->button.button == SDL_BUTTON_MIDDLE) {
			//activate scrolling
			SDL_SetRelativeMouseMode(SDL_TRUE);
			scrolling_active = true;

			//deactivate clicking as long as mousescrolling is active
			clicking_active = false;
		}
		break;
	}

	case SDL_MOUSEBUTTONUP:
		if (scrolling_active and e->button.button == SDL_BUTTON_MIDDLE) {
			//stop scrolling
			SDL_SetRelativeMouseMode(SDL_FALSE);
			scrolling_active = false;

			//reactivate mouse clicks as scrolling is over
			clicking_active = true;
		}
		break;

	case SDL_MOUSEMOTION:

		//scroll, if middle mouse is being pressed
		// SDL_GetRelativeMouseMode() queries sdl for that.
		if (scrolling_active) {

			//move the cam
			coord::vec2f cam_movement {0.0, 0.0};
			cam_movement.x = e->motion.xrel;
			cam_movement.y = e->motion.yrel;

			//this factor controls the scroll speed
			//cam_movement *= 1;

			//calculate camera position delta from velocity and frame duration
			coord::camgame_delta cam_delta;
			cam_delta.x = cam_movement.x;
			cam_delta.y = - cam_movement.y;

			//update camera phys position
			engine::camgame_phys += cam_delta.to_phys3();
		}
		break;

	case SDL_MOUSEWHEEL:
		editor_current_terrain = util::mod<ssize_t>(editor_current_terrain + e->wheel.y, terrain->terrain_id_count);
		break;

	case SDL_KEYUP:
		switch (((SDL_KeyboardEvent *) e)->keysym.sym) {
		case SDLK_ESCAPE:
			//stop the game
			engine::running = false;
			break;
		}

		break;
	case SDL_KEYDOWN:
		switch (((SDL_KeyboardEvent *) e)->keysym.sym) {
		case SDLK_SPACE:
			terrain->blending_enabled = !terrain->blending_enabled;
			break;
		case SDLK_F2:
			engine::save_screenshot("/tmp/openage_00.png");
			break;
		}

		break;
	}

	return true;
}

void move_camera() {
	//read camera movement input keys, and move camera
	//accordingly.

	//camera movement speed, in pixels per millisecond
	//one pixel per millisecond equals 14.3 tiles/second
	float cam_movement_speed_keyboard = 0.5;

	coord::vec2f cam_movement {0.0, 0.0};

	if (engine::input::is_down(SDLK_LEFT)) {
		cam_movement.x -= cam_movement_speed_keyboard;
	}
	if (engine::input::is_down(SDLK_RIGHT)) {
		cam_movement.x += cam_movement_speed_keyboard;
	}
	if (engine::input::is_down(SDLK_DOWN)) {
		cam_movement.y -= cam_movement_speed_keyboard;
	}
	if (engine::input::is_down(SDLK_UP)) {
		cam_movement.y += cam_movement_speed_keyboard;
	}

	cam_movement *= (float) engine::fpscounter->msec_lastframe;

	//calculate camera position delta from velocity and frame duration
	coord::camgame_delta cam_delta;
	cam_delta.x = cam_movement.x;
	cam_delta.y = cam_movement.y;

	//update camera phys position
	engine::camgame_phys += cam_delta.to_phys3();
}


bool on_engine_tick() {
	move_camera();

	return true;
}

bool draw_method() {
	//draw gaben, our great and holy protector, bringer of the half-life 3.
	gaben->draw(coord::camgame {0, 0});

	//draw terrain
	terrain->draw();

	return true;
}

bool hud_draw_method() {
	//draw the currently selected editor texture tile
	terrain->texture(editor_current_terrain)->draw(coord::window{63, 84}.to_camhud());

	return true;
}

} //namespace openage
