#include "openage.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <unistd.h>

#include "engine/engine.h"
#include "engine/texture.h"
#include "engine/shader/shader.h"
#include "engine/shader/program.h"
#include "log/log.h"
#include "util/error.h"
#include "util/filetools.h"
#include "util/timer.h"

namespace openage {

engine::Texture *gaben, *university;

util::Timer *timer;

unsigned lmbx, lmby, rmbx, rmby;

void init() {
	timer = new util::Timer();
	timer->start();

	lmbx = 0;
	lmby = 0;
	rmbx = 0;
	rmby = 0;

	//load textures and stuff
	gaben = new engine::Texture("gaben.png");
	gaben->centered = false;


	//TODO: dynamic generation of the file path
	//sync this with media-convert/age2media.py !

	university = new engine::Texture("age/graphics.drs/003836.slp/003836_000_01.png");

	char *texturevshader_code = util::read_whole_file("shaders/maptexture.vert.glsl");
	engine::teamcolor_shader::vert = new engine::shader::Shader(GL_VERTEX_SHADER, texturevshader_code);
	free(texturevshader_code);

	char *texturefshader_code = util::read_whole_file("shaders/teamcolors.frag.glsl");
	engine::teamcolor_shader::frag = new engine::shader::Shader(GL_FRAGMENT_SHADER, texturefshader_code);
	free(texturefshader_code);

	engine::teamcolor_shader::program = new engine::shader::Program();
	engine::teamcolor_shader::program->attach_shader(engine::teamcolor_shader::vert);
	engine::teamcolor_shader::program->attach_shader(engine::teamcolor_shader::frag);

	engine::teamcolor_shader::program->link();

	engine::teamcolor_shader::player_id_var = engine::teamcolor_shader::program->get_uniform_id("player_number");
	engine::teamcolor_shader::alpha_marker_var = engine::teamcolor_shader::program->get_uniform_id("alpha_marker");
	engine::teamcolor_shader::player_color_var = engine::teamcolor_shader::program->get_uniform_id("player_color");

	//get the player colors from the sub-palette exported by script
	char *pcolor_file = util::read_whole_file("age/player_color_palette.pal");

	const uint num_pcolors = 64;
	GLfloat playercolors[num_pcolors*4];

	char *currentline = pcolor_file;

	for(; *pcolor_file != '\0'; pcolor_file++) {

		if (*pcolor_file == '\n') {
			*pcolor_file = '\0';

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
			currentline = pcolor_file + 1;
		}
	}
	//TODO free the pcolor_file...
	//delete[] pcolor_file;

	engine::teamcolor_shader::program->use();
	//keep in sync with media converter script:
	glUniform1f(engine::teamcolor_shader::alpha_marker_var, 254.0/255.0);
	glUniform4fv(engine::teamcolor_shader::player_color_var, 64, playercolors);
	engine::teamcolor_shader::program->stopusing();

	log::msg("Time for startup: %.4f s", timer->measure()/1000.0);
}

void deinit() {
	log::msg("openage shutting down...");

	delete gaben;
	delete university;
	delete engine::teamcolor_shader::vert;
	delete engine::teamcolor_shader::frag;
	delete engine::teamcolor_shader::program;
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
			lmbx = e->button.x;
			lmby = engine::window_y - e->button.y;
			log::dbg("left button pressed at %d,%d", e->button.x, e->button.y);
		}
		else if (e->button.button == SDL_BUTTON_RIGHT) {
			rmbx = e->button.x;
			rmby = engine::window_y - e->button.y;
			log::dbg("right button pressed at %d,%d", e->button.x, e->button.y);
		}
		break;
	case SDL_KEYUP:
		//switch (e->keysym.sym) {
		//case SDLK_a:
		//	break;
		//}
		break;
	case SDL_KEYDOWN:

		break;
	}
}

void draw_method() {
	gaben->draw(0, 0, 0, false);

	university->draw(lmbx, lmby, 1, false);
	university->draw(rmbx, rmby, 2, true);


}

int mainmethod() {
	//init engine
	engine::init(draw_method, input_handler);
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
