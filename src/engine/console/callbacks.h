#ifndef _ENGINE_CONSOLE_CALLBACKS_H_
#define _ENGINE_CONSOLE_CALLBACKS_H__

#include <SDL2/SDL.h>

namespace engine {
namespace console {

bool on_engine_tick();
bool draw();
bool handle_inputs(SDL_Event *e);
bool on_window_resize();

} //namespace console
} //namespace engine

#endif //_ENGINE_CONSOLE_CALLBACKS_H__
