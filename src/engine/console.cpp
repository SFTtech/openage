#include "console.h"

#include "engine.h"
#include "../util/color.h"
#include "../util/strings.h"

#include <GL/glew.h>
#include <GL/gl.h>

namespace openage {
namespace engine {

Console::Console() : Console(util::Color(255,255,255,255), util::Color(0,0,0,1)) {}

Console::Console(util::Color bg, util::Color text, int lx, int ly, int rx, int ry) : bgcolor(bg), textcolor(text), lx(lx), ly(ly), rx(rx), ry(ry) {}

Console::~Console() {}

void Console::set_winsize(int w, int h) {
	//TODO: maybe use only the top part of the screen
	this->lx = 50;
	this->ly = 50;
	this->rx = w - 50;
	this->ry = h - 50;
}

void Console::set_bgcolor(util::Color newcolor) {
	this->bgcolor = newcolor;
}

void Console::set_textcolor(util::Color newcolor) {
	this->textcolor = newcolor;
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
		//TODO: use glColor4i
		glColor4f(this->bgcolor.r/255.0, this->bgcolor.g/255.0, this->bgcolor.g/255.0, this->bgcolor.a/255.0);

		glBegin(GL_QUADS);
		{
			glVertex2i(lx, ly);
			glVertex2i(rx, ly);
			glVertex2i(rx, ry);
			glVertex2i(lx, ry);
		}
		glEnd();

		glTranslatef(70, 70, 0);

		glColor4f(this->textcolor.r/255.0, this->textcolor.g/255.0, this->textcolor.g/255.0, this->textcolor.a/255.0);
		engine::t_font->Render(util::format("%d gschichten", 1337));

	}
	glPopMatrix();
}

}
}
