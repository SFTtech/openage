#include "input.h"

#include <stdint.h>

#include <unordered_map>

#include "engine.h"
#include "callbacks.h"
#include "util/timer.h"

namespace openage {
namespace input {

std::unordered_map<SDL_Keycode, bool> keydown;

bool is_down(SDL_Keycode k) {
	auto it = keydown.find(k);
	if(it != keydown.end()) {
		return it->second;
	} else {
		keydown[k] = false;
		return false;
	}
}

bool handler(SDL_Event *e) {
	switch(e->type) {
	case SDL_WINDOWEVENT:
		switch(e->window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			//update window size
			window_size.x = e->window.data1;
			window_size.y = e->window.data2;

			//invoke resize callback handlers
			for(auto cb: callbacks::on_resize) {
				if (!cb()) {
					break;
				}
			}
			return false;
			break;
		}
		break;

	case SDL_KEYUP: {
		SDL_Keycode sym = ((SDL_KeyboardEvent *) e)->keysym.sym;
		keydown[sym] = false;
	}
		break;

	case SDL_KEYDOWN: {
		SDL_Keycode sym = ((SDL_KeyboardEvent *) e)->keysym.sym;
		keydown[sym] = true;
	}
		break;
	}

	return true;
}

} //namespace input
} //namespace openage
