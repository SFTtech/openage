#ifndef _ENGINE_ENGINE_H_
#define _ENGINE_ENGINE_H_

#include "console.h"

#include <FTGL/ftgl.h>
#include <SDL2/SDL.h>

namespace openage {
namespace engine {

using draw_method_ptr = void (*) ();
using input_handler_ptr = void (*) (SDL_Event *e);

/**
engine initialization method.
opens a window and initializes the OpenGL context.

@param draw_method
	pointer to the rendering method, which is called each iteration of the main loop
@param input_handler
	pointer to the input handler, which is called each time an input event is registered
*/
void init(draw_method_ptr draw_method, input_handler_ptr input_handler);

/**
engine de-initialization method.
call to cleanly shutdown the application.
*/
void destroy();

/**
main loop method.
terminates when running is set to false.
*/
void loop();

/**
internal method that is automatically called whenever the window is resized.
@param w
	width
@param h
	height
*/
void engine_window_resized(unsigned w, unsigned h);

/**
internal method that is called whenever an input event is registered.
*/
void engine_input_handler(SDL_Event *e);

/**
SDL window of the engine.
*/
extern SDL_Window *window;

/**
the debug console
*/
extern Console *console;

/**
the text font used for (can you believe it?) texts.
*/
extern FTGLTextureFont *t_font;

/**
to be set to false to stop the engine.
*/
extern bool running;

/**
x size of the current window.
*/
extern unsigned window_x;

/**
y size of the current window.
*/
extern unsigned window_y;


} //namespace engine
} //namespace openage

#endif //_ENGINE_ENGINE_H_
