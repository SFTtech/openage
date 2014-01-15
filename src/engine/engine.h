#ifndef _ENGINE_ENGINE_H_
#define _ENGINE_ENGINE_H_

#include <vector>

#include <SDL2/SDL.h>

#include "util/fps.h"
#include "coord/window.h"
#include "coord/phys3.h"
#include "font.h"

namespace engine {

/**
main loop method.
terminates when running is set to false.
*/
void loop();

/**
internal window-resize callback method
*/
bool handle_window_resize();

/**
internal input event callback method
*/
bool handle_input_event(SDL_Event *e);

/**
internal hud drawing method

(draws FPS counter)
*/
bool draw_hud();


void gl_check_error();

/**
SDL window
*/
extern SDL_Window *window;

/**
SDL OpenGL context
*/
extern SDL_GLContext glcontext;

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

#endif //_ENGINE_ENGINE_H_
