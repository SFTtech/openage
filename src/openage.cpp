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

unsigned int terrain_texture_count, blend_mode_count;
unsigned lmbx, lmby, rmbx, rmby;
bool sc_left, sc_right, sc_up, sc_down;

int *terrain_priority_list;

unsigned int terrain_data[20][20] = {
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 11, 11, 11,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 11, 11, 11, 11, 11,  7,  7,  7},
	{  7,  7, 20, 20, 20,  7,  7,  7,  7,  7,  7,  7, 11, 11, 11, 11, 11, 11,  7,  7},
	{  7,  7, 20,  7, 07, 20, 20,  7,  7,  7,  7,  7, 11, 11, 11, 11, 11,  7,  7,  7},
	{  7,  7, 20,  7,  7,  7,  7,  7,  7,  7,  7,  7, 11, 11, 11,  7,  7,  7,  7,  7},
	{  7, 20, 20, 20,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7, 20,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7, 20,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7, 12, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7, 12, 12, 12, 12, 12, 12, 12,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
	{  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7}
};


void init() {
	timer = new util::Timer();
	timer->start();

	lmbx = 0;
	lmby = 0;
	rmbx = 0;
	rmby = 0;

	sc_left = false;
	sc_right = false;
	sc_up = false;
	sc_down = false;

	//load textures and stuff
	gaben = new engine::Texture("gaben.png");
	gaben->centered = false;


	//TODO: dynamic generation of the file path
	//sync this with convert .py script !

	university = new engine::Texture("age/raw/Data/graphics.drs/3836.slp.png", true, true);

	//a list of all terrain slp ids. we'll get them from the .dat files later.
	int terrain_ids[] = {
		15000, 15001, 15002, 15004, 15005,
		15006, 15007, 15008, 15009, 15010,
		15011, 15014, 15015, 15016, 15017,
		15018, 15019, 15021, 15022, 15023,
		15024, 15026, 15027, 15028, 15029,
		15030, 15031
	};

	//hardcoded for now
	blend_mode_count = 9;

	terrain_texture_count = sizeof(terrain_ids)/sizeof(int);
	terrain_priority_list = new int[terrain_texture_count];
	terrain_textures = new engine::Texture*[terrain_texture_count];

	//set terrain priorities, TODO: get them from media files
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

		auto new_texture = new engine::Texture(terraintex_filename, false, true);

		terrain_textures[i] = new_texture;
		terrain->set_texture(i, new_texture);

		delete[] terraintex_filename;
	}

	//add the blendomatic masks to the terrain
	blending_textures = new engine::Texture*[blend_mode_count];

	for (unsigned int i = 0; i < blend_mode_count; i++) {
		char *mask_filename = util::format("age/alphamask/mode%02d.png", i);

		auto new_texture = new engine::Texture(mask_filename, false, true);

		blending_textures[i] = new_texture;
		terrain->set_mask(i, new_texture);

		delete[] mask_filename;
	}


	//set the terrain types according to the data array.
	for (unsigned int i = 0; i < terrain->get_size(); i++) {
		for (unsigned int j = 0; j < terrain->get_size(); j++) {
			int texid = terrain_data[i][j];
			terrain->set_tile_at(texid, i, j);
		}
	}


	char *texturevshader_code = util::read_whole_file("shaders/maptexture.vert.glsl");
	engine::teamcolor_shader::vert = new engine::shader::Shader(GL_VERTEX_SHADER, texturevshader_code);
	delete[] texturevshader_code;

	char *texturefshader_code = util::read_whole_file("shaders/teamcolors.frag.glsl");
	engine::teamcolor_shader::frag = new engine::shader::Shader(GL_FRAGMENT_SHADER, texturefshader_code);
	delete[] texturefshader_code;

	engine::teamcolor_shader::program = new engine::shader::Program();
	engine::teamcolor_shader::program->attach_shader(engine::teamcolor_shader::vert);
	engine::teamcolor_shader::program->attach_shader(engine::teamcolor_shader::frag);

	engine::teamcolor_shader::program->link();

	engine::teamcolor_shader::player_id_var = engine::teamcolor_shader::program->get_uniform_id("player_number");
	engine::teamcolor_shader::alpha_marker_var = engine::teamcolor_shader::program->get_uniform_id("alpha_marker");
	engine::teamcolor_shader::player_color_var = engine::teamcolor_shader::program->get_uniform_id("player_color");

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

	engine::move_view(0, 400);

	log::msg("Time for startup: %.4f s", timer->measure()/1000.0);
}

void deinit() {
	log::msg("openage shutting down...");

	delete gaben;
	delete university;
	delete engine::teamcolor_shader::vert;
	delete engine::teamcolor_shader::frag;
	delete engine::teamcolor_shader::program;

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

	case SDL_MOUSEBUTTONDOWN:
		//a mouse button was pressed...
		//subtract value from window height to get position relative to lower right (0,0).
		if (e->button.button == SDL_BUTTON_LEFT) {
			lmbx = e->button.x - engine::view_x;
			lmby = engine::window_y - e->button.y - engine::view_y;
			log::dbg("left button pressed at %d,%d", lmbx, lmby);
		}
		else if (e->button.button == SDL_BUTTON_RIGHT) {
			rmbx = e->button.x - engine::view_x;
			rmby = engine::window_y - e->button.y - engine::view_y;
			log::dbg("right button pressed at %d,%d", lmbx, lmby);
		}

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

void view_translation() {
	float mx = 0;
	float my = 0;
	if (sc_left) {
		mx += 1;
	}
	if (sc_right) {
		mx -= 1;
	}
	if (sc_up) {
		my -= 1;
	}
	if (sc_down) {
		my += 1;
	}

	float threshold = 0.5;
	float scalefactor = threshold * engine::fpscounter->msec_lastframe;

	mx *= scalefactor;
	my *= scalefactor;

	engine::move_view(mx, my);
}

void draw_method() {
	gaben->draw(0, 0, 0);

	terrain->render();

	university->draw(lmbx, lmby, false, 0, 1);
	university->draw(rmbx, rmby, true, 0, 2);
}

int mainmethod() {
	//init engine
	engine::init(view_translation, draw_method, input_handler);
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
