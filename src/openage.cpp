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
#include "util/fps.h"
#include "util/error.h"

namespace openage {

engine::Texture *gaben, *university;

util::FrameCounter fpscounter;

unsigned lmbx, lmby, rmbx, rmby;

void init() {
	lmbx = 0;
	lmby = 0;
	rmbx = 0;
	rmby = 0;

	//load textures and stuff
	gaben = new engine::Texture("gaben.png");
	gaben->centered = false;

	//TODO: dynamic generation of the file path
	//sync this with media-convert/age2media.py !

	university = new engine::Texture("../resources/age2_generated/graphics.drs/003836.slp/003836_000_01.png");

	engine::teamcolor_shader::vert = new engine::shader::Shader(engine::shader::shader_vertex, "texturevshader");
	engine::teamcolor_shader::vert->load_from_file("./shaders/maptexture.vert.glsl");

	engine::teamcolor_shader::frag = new engine::shader::Shader(engine::shader::shader_fragment, "texturefshader");
	engine::teamcolor_shader::frag->load_from_file("./shaders/teamcolors.frag.glsl");

	engine::teamcolor_shader::vert->compile();
	if (engine::teamcolor_shader::vert->check()) {
		log::err("failed when checking texture vertex shader");
		exit(1);
	}

	engine::teamcolor_shader::frag->compile();
	if (engine::teamcolor_shader::frag->check()) {
		log::err("failed when checking teamcolor fragment shader");
		exit(1);
	}

	engine::teamcolor_shader::program = new engine::shader::Program("teamcolors");
	engine::teamcolor_shader::program->attach_shader(engine::teamcolor_shader::vert);
	engine::teamcolor_shader::program->attach_shader(engine::teamcolor_shader::frag);

	if(engine::teamcolor_shader::program->link()) {
		log::err("failed linking the texture program");
		exit(1);
	}

	engine::teamcolor_shader::player_id_var = engine::teamcolor_shader::program->get_uniform_id("player_number");
	engine::teamcolor_shader::alpha_marker_var = engine::teamcolor_shader::program->get_uniform_id("alpha_marker");

	engine::teamcolor_shader::program->use();
	//TODO do we really need this as a variable?
	//keep in sync with media converter script:
	glUniform1f(engine::teamcolor_shader::alpha_marker_var, 254.0/255.0);
	engine::teamcolor_shader::program->stopusing();
}

void deinit() {
	log::msg("openage shutting down...");
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
	}
}

void draw_method() {
	gaben->draw(0, 0, 0, false);

	university->draw(lmbx, lmby, 1, false);
	university->draw(rmbx, rmby, 2, true);

	fpscounter.frame();
	//TODO: draw text in framebuffer!
	log::msg("fps: %f", fpscounter.fps);
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
	}
}
