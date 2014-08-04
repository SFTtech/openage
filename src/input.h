#ifndef _INPUT_H_
#define _INPUT_H_

#include <SDL2/SDL.h>

namespace openage {
namespace input {

bool is_down(SDL_Keycode k);

bool handler(SDL_Event *e);

} //namespace input
} //namespace openage

#endif //_INPUT_H_
