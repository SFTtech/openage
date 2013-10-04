#include "console.h"

#include <GL/glew.h>
#include <GL/gl.h>

namespace openage {
namespace engine {

Console::Console() : lx(0), ly(0), rx(0), ry(0) {}

Console::~Console() {}

void Console::set_winsize(int w, int h) {
	//TODO: maybe use only the top part of the screen
	this->lx = 50;
	this->ly = 50;
	this->rx = w - 50;
	this->ry = h - 50;
}

void Console::input_handler(SDL_Event *e) {
	//TODO: implement mouse-functionality for selecting _all_ objects

	switch (e->type) {
	case SDL_KEYDOWN:

		break;
	}
}


void Console::draw() {
	glPushMatrix();
	{
		glColor4f(1.0, 1.0, 1.0, 0.5);
		glBegin(GL_QUADS);
		{
			glVertex2i(lx, ly);
			glVertex2i(rx, ly);
			glVertex2i(rx, ry);
			glVertex2i(lx, ry);
		}
		glEnd();
	}
	glPopMatrix();
}

}
}
