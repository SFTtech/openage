#include "openage.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <unistd.h>

#include "engine/engine.h"
#include "engine/texture.h"
#include "engine/shader.h"
#include "log/log.h"
#include "util/fps.h"

namespace openage {

engine::Texture *gaben, *university;

engine::shader::Shader *teamcolor_vert, *teamcolor_frag;
engine::shader::Program *teamcolorshader;
GLint player_texshader_var, alpha_marker_var;

util::FrameCounter fpscounter;

unsigned lmbx, lmby, rmbx, rmby;

void init() {
	lmbx = 0;
	lmby = 0;
	rmbx = 0;
	rmby = 0;

	//load textures and stuff
	gaben = new engine::Texture("gaben.png");

	//TODO: dynamic generation of the file path
	//sync this with media-convert/age2media.py !

	university = new engine::Texture("../resources/age2_generated/graphics.drs/003836.slp/003836_000_01.png");

	teamcolor_vert = new engine::shader::Shader(engine::shader::shader_vertex, "texturevshader");
	teamcolor_vert->load_from_file("./shaders/maptexture.vert.glsl");

	teamcolor_frag = new engine::shader::Shader(engine::shader::shader_fragment, "texturefshader");
	teamcolor_frag->load_from_file("./shaders/teamcolors.frag.glsl");

	teamcolor_vert->compile();
	if (teamcolor_vert->check()) {
		log::err("failed when checking texture vertex shader");
		exit(1);
	}

	teamcolor_frag->compile();
	if (teamcolor_frag->check()) {
		log::err("failed when checking teamcolor fragment shader");
		exit(1);
	}

	teamcolorshader = new engine::shader::Program("teamcolors");
	teamcolorshader->attach_shader(teamcolor_vert);
	teamcolorshader->attach_shader(teamcolor_frag);

	if(teamcolorshader->link()) {
		log::err("failed linking the texture program");
		exit(1);
	}

	player_texshader_var = teamcolorshader->get_uniform_id("player_number");
	alpha_marker_var = teamcolorshader->get_uniform_id("alpha_marker");

	teamcolorshader->use();
	//keep in sync with media converter script:
	glUniform1f(alpha_marker_var, 254.0/255.0);
	teamcolorshader->stopusing();
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
	gaben->draw(0, 0);

	teamcolorshader->use();
	glPushMatrix();
	{
		glColor3f(1, 1, 1);
		glUniform1i(player_texshader_var, 4);
		//this translation equals the hotspot of an image!
		glTranslatef(lmbx - university->w/2, lmby - university->h/2, 0);

		university->draw(0, 0);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glColor3f(1, 1, 1);
		glUniform1i(player_texshader_var, 2);
		glTranslatef(rmbx - university->w/2, rmby - university->h/2, 0);
		university->draw(0, 0);
	}
	glPopMatrix();
	teamcolorshader->stopusing();

	fpscounter.frame();
	//TODO: draw text in framebuffer!
	//log::msg("fps: %f", fpscounter.fps);
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
	return openage::mainmethod();
}
