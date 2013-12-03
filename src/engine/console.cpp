#include "console.h"

#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>
#include <FTGL/ftgl.h>

#include "engine.h"
#include "font.h"
#include "log.h"
#include "util/color.h"
#include "util/error.h"
#include "util/strings.h"

namespace engine {

Console::Console() : Console(util::col {255, 255, 255, 255}, util::col {0, 0, 0, 1}) {}

Console::Console(util::col bg, util::col text, int lx, int ly, int rx, int ry) : bgcolor(bg), textcolor(text), lx(lx), ly(ly), rx(rx), ry(ry), spacing(7) {

	font = new Font("DejaVu Sans Mono", "Book", 12);
}

Console::~Console() {
	delete this->font;

	for (auto elem : this->messages) {
		delete[] elem;
	}
	this->messages.clear();
}

void Console::set_winsize(int w, int h) {
	//TODO: maybe use only the top part of the screen
	this->lx = 50;
	this->ly = 50;
	this->rx = w - 50;
	this->ry = h - 50;
}

void Console::set_bgcolor(util::col newcolor) {
	this->bgcolor = newcolor;
}

void Console::set_textcolor(util::col newcolor) {
	this->textcolor = newcolor;
}

void Console::add_message(const char *text) {
	char *store_text = util::copy(text);
	this->messages.push_back(store_text);
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
		bgcolor.use();

		glBegin(GL_QUADS);
		{
			glVertex2i(this->lx, this->ly);
			glVertex2i(this->rx, this->ly);
			glVertex2i(this->rx, this->ry);
			glVertex2i(this->lx, this->ry);
		}
		glEnd();

		textcolor.use();
		glTranslatef(lx + 10, ly + 10, 0);

		int px = 0, py = 0;

		//display all stored messages
		for (auto msg = this->messages.crbegin(); msg != this->messages.crend(); ++msg) {
			const char *cmsg = *msg;

			//get the font metrics
			FTBBox bbox = this->font->internal_font->BBox(cmsg);

			FTPoint lower = bbox.Lower();
			FTPoint upper = bbox.Upper();

			//TODO: linewrap if text too wide
			//float w = upper.Xf() - lower.Xf();
			float h = upper.Yf() - lower.Yf();

			//stop displaying older messages no longer fitting on screen
			if ((py + h + this->spacing) > (this->ry - this->ly)) {
				break;
			}

			this->font->render_static(px, py, cmsg);

			py += this->spacing + h;
		}

	}
	glPopMatrix();
}

} //namespace console
