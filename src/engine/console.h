#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "../util/color.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

namespace openage {
namespace engine {

class Console {
private:
	int lx, ly, rx, ry;
	util::Color bgcolor, textcolor;

public:
	Console();
	~Console();

	void set_winsize(int w, int h);
	void draw();

	void input_handler(SDL_Event *e);
};

} //namespace engine
} //namespace openage

#endif //_CONSOLE_H_
