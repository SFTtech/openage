#ifndef _ENGINE_ENGINE_H_
#define _ENGINE_ENGINE_H_

#include <FTGL/ftgl.h>
#include <SDL2/SDL.h>

#include "console.h"
#include "../util/fps.h"


namespace openage {
namespace engine {

using noparam_method_ptr = void (*) ();
using input_handler_ptr = void (*) (SDL_Event *e);

/**
engine initialization method.
opens a window and initializes the OpenGL context.
@param view_translation_method
	pointer to a method that translates the engine view
@param draw_method
	pointer to the rendering method, which is called each iteration of the main loop
@param input_handler
	pointer to the input handler, which is called each time an input event is registered
*/
void init(noparam_method_ptr view_translation_method, noparam_method_ptr draw_method, input_handler_ptr input_handler);

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
move the view by a given amount of pixels.
*/
void move_view(int delta_x, int delta_y);

/**
move the view by a threshold [0..1] in direction of <x,y> (x=[0..1], y=[0..1])
*/
void move_view(float threshold, float x, float y);

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

/**
view position x.
This represents the scrolling x position of the user.
*/
extern int view_x;

/**
view position y.
This represents the scrolling y position of the user.
*/
extern int view_y;

/**
positions visible in viewport.
*/
extern int visible_x_left, visible_x_right, visible_y_top, visible_y_bottom;

/**
the frame counter measuring fps.
*/
extern util::FrameCounter *fpscounter;


} //namespace engine
} //namespace openage

#endif //_ENGINE_ENGINE_H_
