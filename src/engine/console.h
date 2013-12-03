#ifndef _ENGINE_CONSOLE_H_
#define _ENGINE_CONSOLE_H_

#include "util/color.h"

#include <vector>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include "font.h"

namespace engine {

class Console {
private:
	util::col bgcolor, textcolor;
	int lx, ly, rx, ry;
	std::vector<const char *> messages;
	Font *font;

	int spacing;

public:
	Console();
	Console(util::col bg, util::col fg, int lx=0, int ly=0, int rx=0, int ry=0);
	~Console();

	void set_winsize(int w, int h);
	void set_bgcolor(util::col newcolor);
	void set_textcolor(util::col newcolor);

	void add_message(const char *text);

	void draw();

	void input_handler(SDL_Event *e);
};

} //namespace engine

#endif //_ENGINE_CONSOLE_H_
