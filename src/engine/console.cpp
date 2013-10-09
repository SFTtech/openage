#include "console.h"

#include "engine.h"
#include "../util/color.h"
#include "../util/error.h"
#include "../util/strings.h"
#include "../log/log.h"

#include <vector>
#include <string>
#include <GL/glew.h>
#include <GL/gl.h>
#include <FTGL/ftgl.h>

namespace openage {
namespace engine {

Console::Console() : Console(util::Color(255,255,255,255), util::Color(0,0,0,1)) {}

Console::Console(util::Color bg, util::Color text, int lx, int ly, int rx, int ry) : bgcolor(bg), textcolor(text), lx(lx), ly(ly), rx(rx), ry(ry), spacing(7) {
	this->font = new FTGLTextureFont("/usr/share/fonts/dejavu/DejaVuSerif.ttf");

	if(this->font->Error())
		throw util::Error("failed creating the console font");

	this->font->FaceSize(12);
}

Console::~Console() {
	delete this->font;
}

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

void Console::add_message(std::string text) {
	this->messages.push_back(text);
}

void Console::add_message(const char *text) {
	this->add_message(std::string(text));
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
			glVertex2i(this->lx, this->ly);
			glVertex2i(this->rx, this->ly);
			glVertex2i(this->rx, this->ry);
			glVertex2i(this->lx, this->ry);
		}
		glEnd();

		glColor4f(this->textcolor.r/255.0, this->textcolor.g/255.0, this->textcolor.g/255.0, this->textcolor.a/255.0);
		glTranslatef(lx + 10, ly + 10, 0);


		int px = 0, py = 0;

		//display all stored messages
		for (auto msg = this->messages.crbegin(); msg != this->messages.crend(); ++msg) {
			const char *cmsg = msg->c_str();

			//get the font metrics
			FTBBox bbox = this->font->BBox(cmsg);

			FTPoint lower = bbox.Lower();
			FTPoint upper = bbox.Upper();

			//TODO: linewrap if text too wide
			//float w = upper.Xf() - lower.Xf();
			float h = upper.Yf() - lower.Yf();

			//stop displaying older messages no longer fitting on screen
			if ((py + h + this->spacing) > (this->ry - this->ly)) {
				break;
			}

			this->font->Render(cmsg, -1, FTPoint(px, py));

			py += this->spacing + h;
		}

	}
	glPopMatrix();
}

}
}
