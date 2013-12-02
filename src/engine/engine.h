#ifndef _ENGINE_ENGINE_H_
#define _ENGINE_ENGINE_H_

#include <SDL2/SDL.h>

#include "console.h"
#include "../util/fps.h"
#include "coord/window.h"
#include "coord/phys3.h"
#include "font.h"


namespace openage {
namespace engine {

using noparam_method_ptr = void (*) ();
using input_handler_ptr = void (*) (SDL_Event *e);

/**
engine initialization method.
opens a window and initializes the OpenGL context.
@param on_engine_tick
	pointer to a method that is executed on every engine tick (after input handling, before rendering)
@param draw_method
	pointer to the drawing method, which is called each iteration of the main loop,
	with coord_camera set as the OpenGL coordinate system
@param hud_draw_method
	pointer to the rendering method, which is called each iteration of the main loop,
	with coord_hud set as the OpenGL coordinate system
@param input_handler
	pointer to the input handler, which is called each time an input event is registered
*/
void init(noparam_method_ptr on_engine_tick, noparam_method_ptr draw_method, noparam_method_ptr hud_draw_method, input_handler_ptr input_handler);

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
the text fonts to be used for (can you believe it?) texts.
loaded on engine init.
*/
namespace fonts {
/**
dejavu serif, book, 20pts
*/
extern Font *dejavuserif20;
}

/**
to be set to false to stop the engine.
*/
extern bool running;

/**
size of the game window, in coord_sdl
*/
extern coord::window window_size;

/**
position of the game camera, in the phys3 system
(the position that it is rendered at camgame {0, 0})
*/
extern coord::phys3 camgame_phys;
/**
position of the game camera, in the window system
(the position where the camgame {0, 0} is rendered)
*/
extern coord::window camgame_window;

/**
position of the hud camera, in the window system
(the position where camhud {0, 0} is rendered)
*/
extern coord::window camhud_window;

/**
the frame counter measuring fps.
*/
extern util::FrameCounter *fpscounter;

} //namespace engine
} //namespace openage

#endif //_ENGINE_ENGINE_H_
