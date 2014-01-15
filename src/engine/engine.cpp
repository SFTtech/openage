#include "engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <FTGL/ftgl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "callbacks.h"
#include "texture.h"
#include "log.h"
#include "util/error.h"
#include "util/fps.h"
#include "util/strings.h"
#include "util/color.h"


/**
* stores all things that have to do with the game engine.
*
* this includes textures, objects, terrain, etc.
* movement and transformation stuff, and actually everything that
* makes the game work lies in here...
*/
namespace engine {

//global engine variables; partially accesible via engine.h
SDL_GLContext glcontext;
SDL_Window *window;

namespace fonts {
Font *dejavuserif20;
}

bool running;

coord::window window_size = {800, 600};
coord::phys3 camgame_phys = {10 * coord::phys_per_tile, 10 * coord::phys_per_tile, 0};
coord::window camgame_window = {400, 300};
coord::window camhud_window = {0, 600};

util::FrameCounter *fpscounter;

/**
* update opengl when the window is resized.
*
* updates transformation matrices and the viewport size.
*/
bool handle_window_resize() {
	//update camgame window position
	camgame_window = window_size / 2;
	//update camhud window position
	camhud_window = {0, (coord::pixel_t) window_size.y};

	//update OpenGL viewport and projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, window_size.x, window_size.y);

	//set orthographic projection: left, right, bottom, top, near_val, far_val
	glOrtho(0, window_size.x, 0, window_size.y, 9001, -1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	log::dbg("engine window has been resized to %hdx%hd\n", window_size.x, window_size.y);

	return true;
}

bool draw_hud() {
	//draw FPS counter
	util::col {255, 255, 255, 255}.use();
	fonts::dejavuserif20->render(window_size.x - 100, 15, "%.1f fps", fpscounter->fps);

	return true;
}

/**
the main engine loop.

the loop invokes fps counting, SDL event handling,
view translation, and calling the main draw_method.
*/
void loop() {
	running = true;
	SDL_Event e;

	while (running) {
		fpscounter->frame();

		while (SDL_PollEvent(&e)) {
			//call the input callback methods
			for(auto cb: callbacks::on_input) {
				if (!cb(&e)) {
					break;
				};
			}
		}

		//call engine tick callback methods
		for(auto cb: callbacks::on_engine_tick) {
			cb();
		}

		//clear the framebuffer to black
		//in the future, we might disable it for lazy drawing
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glPushMatrix(); {
			//set the framebuffer up for camgame rendering
			glTranslatef(camgame_window.x, camgame_window.y, 0);

			//invoke all game drawing callback methods
			for(auto cb: callbacks::on_drawgame) {
				if (!cb()) {
					break;
				};
			}
		}
		glPopMatrix();

		glPushMatrix(); {
			//the hud coordinate system is automatically established

			//invoke all hud drawing callback methods
			for(auto cb: callbacks::on_drawhud) {
				if (!cb()) {
					break;
				};
			}
		}
		glPopMatrix();

		gl_check_error();

		//the rendering is done
		//swap the drawing buffers to actually show the frame
		SDL_GL_SwapWindow(window);
	}
}


void gl_check_error() {
	int glerrorstate = 0;

	glerrorstate = glGetError();
	if (glerrorstate != GL_NO_ERROR) {

		const char *errormsg;

		//generate error message
		switch (glerrorstate) {
		case GL_INVALID_ENUM:
			//An unacceptable value is specified for an enumerated argument.
			//The offending command is ignored
			//and has no other side effect than to set the error flag.
			errormsg = "invalid enum passed to opengl call";
			break;
		case GL_INVALID_VALUE:
			//A numeric argument is out of range.
			//The offending command is ignored
			//and has no other side effect than to set the error flag.
			errormsg = "invalid value passed to opengl call";
			break;
		case GL_INVALID_OPERATION:
			//The specified operation is not allowed in the current state.
			//The offending command is ignored
			//and has no other side effect than to set the error flag.
			errormsg = "invalid operation performed during some state";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			//The framebuffer object is not complete. The offending command
			//is ignored and has no other side effect than to set the error flag.
			errormsg = "invalid framebuffer operation";
			break;
		case GL_OUT_OF_MEMORY:
			//There is not enough memory left to execute the command.
			//The state of the GL is undefined,
			//except for the state of the error flags,
			//after this error is recorded.
			errormsg = "out of memory, wtf?";
			break;
		case GL_STACK_UNDERFLOW:
			//An attempt has been made to perform an operation that would
			//cause an internal stack to underflow.
			errormsg = "stack underflow";
			break;
		case GL_STACK_OVERFLOW:
			//An attempt has been made to perform an operation that would
			//cause an internal stack to overflow.
			errormsg = "stack overflow";
			break;
		default:
			//unknown error state
			errormsg = "unknown error";
		}
		throw Error("OpenGL error state after running draw method: %d\n\t%s", glerrorstate, errormsg);
	}
}

} //namespace engine
