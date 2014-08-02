#ifndef _ENGINE_CALLBACKS_H_
#define _ENGINE_CALLBACKS_H_

#include <vector>

#include <SDL2/SDL.h>

namespace engine {

using tick_method_ptr = bool (*) ();
using drawgame_method_ptr = bool (*) ();
using drawhud_method_ptr = bool (*) ();
using input_handler_ptr = bool (*) (SDL_Event *e);
using resize_handler_ptr = bool (*) ();

namespace callbacks {

/**
 * run every time an input event is registered
 *
 * if false is returned, no further handlers are run
 */
extern std::vector<input_handler_ptr> on_input;

/**
 * run on every engine tick, after input handling, before rendering
 *
 * if false is returned, no further handlers are run
 */
extern std::vector<tick_method_ptr> on_engine_tick;

/**
 * run every time the game is being drawn,
 * with the renderer set to the camgame system
 *
 * if false is returned, no further handlers are run
 */
extern std::vector<drawgame_method_ptr> on_drawgame;

/**
 * run every time the hud is being drawn,
 * with the renderer set to the camhud system
 *
 * if false is returned, no further handlers are run
 */
extern std::vector<drawhud_method_ptr> on_drawhud;

/**
 * run every time the window is being resized
 *
 * if false is returned, no further handlers are run
 */
extern std::vector<resize_handler_ptr> on_resize;

} //namespace callbacks

} //namespace engine

#endif //_ENGINE_CALLBACKS_H_
