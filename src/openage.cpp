#include "openage.h"

#include <SDL2/SDL.h>
#include <GL/gl.h>

#include "engine/engine.h"
#include "engine/texture.h"
#include "log/log.h"
#include "util/fps.h"

namespace openage {

engine::Texture *gaben, *town_center, *castle_0;

util::FrameCounter fpscounter;

unsigned lmbx, lmby, rmbx, rmby;

void init() {
	lmbx = 0;
	lmby = 0;
	rmbx = 0;
	rmby = 0;
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
		glTranslatef(lmbx - town_center->w/2, lmby - town_center->h/2, 0);

		town_center->draw(0, 0);
	}
	glPopMatrix();
	glPushMatrix();
	{
		glColor3f(1, 1, 1);
		glTranslatef(rmbx - town_center->w/2, rmby - town_center->h/2, 0);

		castle_0->draw(0, 0);
	}
	glPopMatrix();
	/*
	glBegin(GL_QUADS);
	{
		glVertex3f(0, 0, 0);
		glVertex3f(0, 100, 0);
		glVertex3f(100, 100, 0);
		glVertex3f(100, 0, 0);
	}
	glEnd();
	*/
	fpscounter.frame();
	//log::msg("fps: %f", fpscounter.fps);
}

int mainmethod() {
	//init engine
	engine::init(draw_method, input_handler);
	init();

	//load textures and stuff
	gaben = new engine::Texture("gaben.png");

	town_center = new engine::Texture("base.png");
	castle_0 = new engine::Texture("castle_0.png");

	//run main loop
	engine::loop();

	//de-init engine
	engine::destroy();

	return 0;
}

} //namespace openage

int main() {
	return openage::mainmethod();
}
