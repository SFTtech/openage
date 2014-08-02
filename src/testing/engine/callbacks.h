#ifndef _OPENAGE_CALLBACKS_H_
#define _OPENAGE_CALLBACKS_H_

#include <SDL2/SDL.h>

namespace openage {

bool on_engine_tick();
bool draw_method();
bool hud_draw_method();
bool input_handler(SDL_Event *e);

} //namespace openage

#endif //_OPENAGE_CALLBACKS_H_
