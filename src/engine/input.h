#ifndef _ENGINE_INPUT_H_
#define _ENGINE_INPUT_H_

#include <SDL2/SDL.h>

namespace engine {
namespace input {

bool is_down(SDL_Keycode k);

bool handler(SDL_Event *e);

} //namespace input
} //namespace engine

#endif //_ENGINE_INPUT_H_
