#include "openage.h"

#include <SDL2/SDL.h>
#include <GL/gl.h>

#include "engine/engine.h"
#include "engine/texture.h"
#include "log/log.h"
#include "util/fps.h"

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

	//TODO: dynamic generation of the file path
	//sync this with media-convert/age2media.py !

	university = new engine::Texture("../resources/age2_generated/graphics.drs/003836.slp/003836_000_04.png");
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


	glPushMatrix();
	{
		glColor3f(1, 1, 1);
		//this translation equals the hotspot of an image!
		glTranslatef(lmbx - university->w/2, lmby - university->h/2, 0);

		university->draw(0, 0);
	}
	glPopMatrix();

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
