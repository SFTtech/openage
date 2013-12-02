#include "openage.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <unistd.h>

#include "engine/engine.h"
#include "engine/terrain.h"
#include "engine/texture.h"
#include "engine/shader/shader.h"
#include "engine/shader/program.h"
#include "engine/coord/tile3.h"
#include "engine/coord/tile.h"
#include "engine/coord/window.h"
#include "engine/coord/phys3.h"
#include "log/log.h"
#include "util/error.h"
#include "util/filetools.h"
#include "util/misc.h"
#include "util/strings.h"
#include "util/timer.h"

namespace openage {

engine::Texture *gaben, *university;
engine::Texture **terrain_textures, **blending_textures;

engine::Terrain *terrain;

util::Timer *timer;

unsigned int blend_mode_count;
bool sc_left, sc_right, sc_up, sc_down;

int *terrain_priority_list;
struct building {
	engine::coord::tile pos;
	unsigned player;
	engine::Texture *tex;
};

std::vector<building> buildings;

size_t editor_current_terrain = 0;

unsigned int terrain_data[20][20] = {
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 11, 11, 11,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 11, 11, 11, 11, 11,  7,  7,  7},
	{  7,  7, 20, 20, 20,  7,  7,  7,  7,  7,  7,  7, 11, 11, 11, 11, 11, 11,  7,  7},
	{  7,  7, 20,  7,  7, 20, 20,  7,  7,  7,  7,  7, 11, 11, 11, 11, 11,  7,  7,  7},
	{  7,  7, 20,  7,  7,  7,  7,  7,  7,  7,  7,  7, 11, 11, 11,  7,  7,  7,  7,  7},
	{  7, 20, 20, 20,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7, 20,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7, 20,  7,  7,  7,  9,  9,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  9,  9,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7, 13,  7,  9,  7,  7, 12, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7, 13,  9,  9,  7, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7, 13,  7,  7,  7, 12,  7,  7,  7,  7, 17, 17, 17,  7,  7,  7,  7},
	{  7,  7,  7,  7, 13,  7,  7,  7, 12,  7,  7,  7,  7, 18, 18, 18,  7,  7,  7,  7},
	{  7,  7, 12, 12, 12, 12, 12, 12, 12,  7,  7,  7,  7, 19, 19, 19,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  3,  3,  3,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  3,  3,  3,  3, 14, 14,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  3,  3,  7,  7,  7}
};

//a list of all terrain slp ids. we'll get them from the .dat files later.
constexpr int terrain_ids[] = {
	15000, 15001, 15002, 15004, 15005,
	15006, 15007, 15008, 15009, 15010,
	15011, 15014, 15015, 15016, 15017,
	15018, 15019, 15021, 15022, 15023,
	15024, 15026, 15027, 15028, 15029,
	15030, 15031
};

constexpr size_t terrain_texture_count = sizeof(terrain_ids)/sizeof(int);

void init() {
	timer = new util::Timer();
	timer->start();

	//TODO move input tracking into engine
	sc_left = false;
	sc_right = false;
	sc_up = false;
	sc_down = false;

	//load textures and stuff
	gaben = new engine::Texture("gaben.png");

	//TODO: dynamic generation of the file path
	//sync this with convert .py script !

	university = new engine::Texture("age/raw/Data/graphics.drs/3836.slp.png", true, engine::PLAYERCOLORED);

	//hardcoded for now
	blend_mode_count = 9;

	terrain_priority_list = new int[terrain_texture_count];
	terrain_textures = new engine::Texture*[terrain_texture_count];

	//set terrain priorities, TODO: get them from media files. hardcoded for now.
	terrain_priority_list[0]  = 70;
	terrain_priority_list[1]  = 102;
	terrain_priority_list[2]  = 139;
	terrain_priority_list[3]  = 155;
	terrain_priority_list[4]  = 157;
	terrain_priority_list[5]  = 101;
	terrain_priority_list[6]  = 106;
	terrain_priority_list[7]  = 90;
	terrain_priority_list[8]  = 100;
	terrain_priority_list[9]  = 80;
	terrain_priority_list[10] = 92;
	terrain_priority_list[11] = 60;
	terrain_priority_list[12] = 140;
	terrain_priority_list[13] = 141;
	terrain_priority_list[14] = 110;
	terrain_priority_list[15] = 122;
	terrain_priority_list[16] = 123;
	terrain_priority_list[17] = 150;
	terrain_priority_list[18] = 151;
	terrain_priority_list[19] = 152;
	terrain_priority_list[20] = 40;
	terrain_priority_list[21] = 130;
	terrain_priority_list[22] = 132;
	terrain_priority_list[23] = 134;
	terrain_priority_list[24] = 136;
	terrain_priority_list[25] = 162;
	terrain_priority_list[26] = 120;

	terrain = new engine::Terrain(20, terrain_texture_count, blend_mode_count, terrain_priority_list);

	for (unsigned int i = 0; i < terrain_texture_count; i++) {
		int current_id = terrain_ids[i];
		char *terraintex_filename = util::format("age/raw/Data/terrain.drs/%d.slp.png", current_id);

		auto new_texture = new engine::Texture(terraintex_filename, true, engine::ALPHAMASKED);
		new_texture->fix_hotspots(48, 24);

		terrain_textures[i] = new_texture;
		terrain->set_texture(i, new_texture);

		delete[] terraintex_filename;
	}

	//add the blendomatic masks to the terrain
	blending_textures = new engine::Texture*[blend_mode_count];

	for (unsigned int i = 0; i < blend_mode_count; i++) {
		char *mask_filename = util::format("age/alphamask/mode%02d.png", i);

		auto new_texture = new engine::Texture(mask_filename, true);
		new_texture->fix_hotspots(48, 24);

		blending_textures[i] = new_texture;
		terrain->set_mask(i, new_texture);

		delete[] mask_filename;
	}


	//set the terrain types according to the data array.
	engine::coord::tile pos = {0, 0};
	for (; pos.ne < (int) terrain->get_size(); pos.ne++) {
		for (pos.se = 0; pos.se < (int) terrain->get_size(); pos.se++) {
			int texid = terrain_data[pos.ne][pos.se];
			terrain->set_tile(pos, texid);
		}
	}


	char *texturevshader_code = util::read_whole_file("shaders/maptexture.vert.glsl");
	auto maptexture = new engine::shader::Shader(GL_VERTEX_SHADER, texturevshader_code);
	delete[] texturevshader_code;

	char *teamcolor_code = util::read_whole_file("shaders/teamcolors.frag.glsl");
	auto teamcolor_frag = new engine::shader::Shader(GL_FRAGMENT_SHADER, teamcolor_code);
	delete[] teamcolor_code;

	char *alphamask_vert_code = util::read_whole_file("shaders/alphamask.vert.glsl");
	auto alphamask_vert = new engine::shader::Shader(GL_VERTEX_SHADER, alphamask_vert_code);
	delete[] alphamask_vert_code;

	char *alphamask_frag_code = util::read_whole_file("shaders/alphamask.frag.glsl");
	auto alphamask_frag = new engine::shader::Shader(GL_FRAGMENT_SHADER, alphamask_frag_code);
	delete[] alphamask_frag_code;


	engine::teamcolor_shader::program = new engine::shader::Program(maptexture, teamcolor_frag);
	engine::teamcolor_shader::program->link();
	engine::teamcolor_shader::player_id_var = engine::teamcolor_shader::program->get_uniform_id("player_number");
	engine::teamcolor_shader::alpha_marker_var = engine::teamcolor_shader::program->get_uniform_id("alpha_marker");
	engine::teamcolor_shader::player_color_var = engine::teamcolor_shader::program->get_uniform_id("player_color");

	engine::alphamask_shader::program = new engine::shader::Program(alphamask_vert, alphamask_frag);
	engine::alphamask_shader::program->link();
	engine::alphamask_shader::base_texture = engine::alphamask_shader::program->get_uniform_id("base_texture");
	engine::alphamask_shader::mask_texture = engine::alphamask_shader::program->get_uniform_id("mask_texture");
	//engine::alphamask_shader::pos_id       = engine::alphamask_shader::program->get_attribute_id("vposition");
	//engine::alphamask_shader::base_coord   = engine::alphamask_shader::program->get_attribute_id("btexc");
	//engine::alphamask_shader::mask_coord   = engine::alphamask_shader::program->get_attribute_id("mtexc");

	//GLuint ash_id = engine::alphamask_shader::program->get_id();
	//GLint vpos_id = glGetAttribLocation(ash_id, "vposition")


	//get the player colors from the sub-palette exported by script
	char *pcolor_file = util::read_whole_file("age/processed/player_color_palette.pal");

	char *pcolor_seeker = pcolor_file;
	char *currentline = pcolor_file;

	//hardcoded for now.
	const uint num_pcolors = 64;
	GLfloat playercolors[num_pcolors*4];

	for(; *pcolor_seeker != '\0'; pcolor_seeker++) {

		if (*pcolor_seeker == '\n') {
			*pcolor_seeker = '\0';

			if (*currentline != '#') {
				uint n, r, g, b, a, idx;

				//TODO raise exception if stuff is specified multiple times.
				//alternatively, simply dont use indices, raise exception if values are > 255 or < 0, or if idx is < 0 or > 63

				if(sscanf(currentline, "n=%u", &n) == 1) {
					if (n != num_pcolors) {
						throw util::Error("the player colortable must have %u entries!", num_pcolors);
					}
				} else if(sscanf(currentline, "%u=%u,%u,%u,%u", &idx, &r, &g, &b, &a)) {
					playercolors[idx*4] = r / 255.0;
					playercolors[idx*4 + 1] = g / 255.0;
					playercolors[idx*4 + 2] = b / 255.0;
					playercolors[idx*4 + 3] = a / 255.0;
				} else {
					throw util::Error("unknown line content reading the player color table");
				}
			}
			currentline = pcolor_seeker + 1;
		}
	}

	delete[] pcolor_file;

	engine::teamcolor_shader::program->use();
	//keep in sync with media converter script:
	glUniform1f(engine::teamcolor_shader::alpha_marker_var, 254.0/255.0);
	glUniform4fv(engine::teamcolor_shader::player_color_var, 64, playercolors);
	engine::teamcolor_shader::program->stopusing();

	engine::alphamask_shader::program->use();
	glUniform1i(engine::alphamask_shader::base_texture, 0);
	glUniform1i(engine::alphamask_shader::mask_texture, 1);
	engine::alphamask_shader::program->stopusing();

	log::msg("Time for startup: %.4f s", timer->measure()/1000.0);
}

void deinit() {
	log::msg("openage shutting down...");

	delete gaben;
	delete university;
	delete engine::shared_shaders::maptexture;
	delete engine::teamcolor_shader::frag;
	delete engine::teamcolor_shader::program;
	delete engine::alphamask_shader::frag;
	delete engine::alphamask_shader::program;

	for (unsigned int i = 0; i < terrain_texture_count; i++) {
		delete terrain_textures[i];
	}
	for (unsigned int i = 0; i < blend_mode_count; i++) {
		delete blending_textures[i];
	}
	delete[] terrain_priority_list;
}

void input_handler(SDL_Event *e) {
	switch (e->type) {

	case SDL_QUIT:
		engine::running = false;
		break;

	case SDL_MOUSEBUTTONDOWN: {
		using namespace engine;
		using namespace engine::coord;

		//a mouse button was pressed...
		//subtract value from window height to get position relative to lower right (0,0).
		coord::window mousepos_window {(pixel_t) e->button.x, (pixel_t) e->button.y};
		coord::camgame mousepos_camgame = mousepos_window.to_camgame();
		//TODO once the terrain elevation milestone is implemented, use a method
		//more suitable for converting camgame to phys3
		coord::phys3 mousepos_phys3 = mousepos_camgame.to_phys3();
		coord::tile mousepos_tile = mousepos_phys3.to_tile3().to_tile();

		if (e->button.button == SDL_BUTTON_LEFT) {
			log::dbg("LMB [window]:    x %9hd y %9hd", mousepos_window.x, mousepos_window.y);
			log::dbg("LMB [camgame]:   x %9hd y %9hd", mousepos_camgame.x, mousepos_camgame.y);
			log::dbg("LMB [phys3]:     NE %8.3f SE %8.3f UP %8.3f",
				((float) mousepos_phys3.ne) / phys_per_tile,
				((float) mousepos_phys3.se) / phys_per_tile,
				((float) mousepos_phys3.up) / phys_per_tile);
			log::dbg("LMB [tile]:      NE %8ld SE %8ld", mousepos_tile.ne, mousepos_tile.se);
			terrain->set_tile(mousepos_tile, editor_current_terrain);
		}
		else if (e->button.button == SDL_BUTTON_RIGHT) {
			//check whether an building already exists at this pos
			bool found = false;

			for(unsigned i = 0; i < buildings.size(); i++) {
				if (buildings[i].pos == mousepos_tile) {
					buildings.erase(buildings.begin() + i);
					found = true;
					break;
				}
			}

			if(!found) {
				building newbuilding;
				newbuilding.player = 1 + (buildings.size() % 8);
				newbuilding.pos = mousepos_tile;
				newbuilding.tex = university;
				buildings.push_back(newbuilding);
			}
		}

		}
		break;
	case SDL_MOUSEWHEEL:
		editor_current_terrain = util::mod<size_t>(editor_current_terrain + e->wheel.y, terrain_texture_count);
		break;

	case SDL_KEYUP:
		switch (((SDL_KeyboardEvent *) e)->keysym.sym) {
		case SDLK_LEFT:
			sc_left = false;
			break;
		case SDLK_RIGHT:
			sc_right = false;
			break;
		case SDLK_UP:
			sc_up = false;
			break;
		case SDLK_DOWN:
			sc_down = false;
			break;
		case SDLK_ESCAPE:
			//stop the game
			engine::running = false;
			break;
		}

		break;
	case SDL_KEYDOWN:
		switch (((SDL_KeyboardEvent *) e)->keysym.sym) {

		case SDLK_LEFT:
			sc_left = true;
			break;
		case SDLK_RIGHT:
			sc_right = true;
			break;
		case SDLK_UP:
			sc_up = true;
			break;
		case SDLK_DOWN:
			sc_down = true;
			break;
		case SDLK_SPACE:
			terrain->blending_enabled = !terrain->blending_enabled;
			break;
		}

		break;
	}
}

void move_camera() {
	//read camera movement input keys, and move camera
	//accordingly.

	//camera movement speed, in pixels per millisecond
	//one pixel per millisecond equals 14.3 tiles/second
	float camera_movement_speed_keyboard = 0.5;

	engine::coord::camgame_delta camera_delta = {0, 0};
	if (sc_left) {
		camera_delta.x -= camera_movement_speed_keyboard;
	}
	if (sc_right) {
		camera_delta.x += camera_movement_speed_keyboard;
	}
	if (sc_up) {
		camera_delta.y -= camera_movement_speed_keyboard;
	}
	if (sc_down) {
		camera_delta.y += camera_movement_speed_keyboard;
	}

	//calculate camera position delta from velocity and frame duration
	camera_delta *= (float) engine::fpscounter->msec_lastframe;

	//update camera phys position
	engine::camgame_phys += camera_delta.to_phys3();
}

void on_engine_tick() {
	move_camera();
}

void draw_method() {
	//draw gaben, our great and holy protector, bringer of the half-life 3.
	gaben->draw(engine::coord::camgame {0, 0});

	//draw terrain
	terrain->draw();

	//draw each building
	for(auto &building : buildings){
		building.tex->draw(building.pos.to_tile3().to_phys3().to_camgame(), false, 0, building.player);
	}
}

void hud_draw_method() {
	//draw the currently selected editor texture tile
	terrain->get_texture(editor_current_terrain)->draw(engine::coord::window{63, 84}.to_camhud());
}

int mainmethod() {
	//init engine
	engine::init(on_engine_tick, draw_method, hud_draw_method, input_handler);
	init();

	//run main loop
	engine::loop();

	//de-init engine
	engine::destroy();
	deinit();

	return 0;
}

} //namespace openage

int main() {
	try {
		return openage::mainmethod();
	} catch (openage::util::Error e) {
		openage::log::fatal("Exception: %s", e.str());
		return 1;
	}
}
