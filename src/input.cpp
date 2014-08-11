#include "input.h"

#include <stdint.h>

#include "engine.h"
#include "callbacks.h"
#include "util/timer.h"

namespace openage {

CoreInputHandler::CoreInputHandler() {}
CoreInputHandler::~CoreInputHandler() {}


// not that the function stores a unknown/new keycode
// as 'not pressed' when it was requested
bool CoreInputHandler::is_key_down(SDL_Keycode k) {
	auto it = this->key_states.find(k);
	if (it != this->key_states.end()) {
		return it->second;
	} else {
		this->key_states[k] = false;
		return false;
	}
}

bool CoreInputHandler::on_input(SDL_Event *event) {
	switch (event->type) {
	case SDL_WINDOWEVENT:
		switch(event->window.event) {
		case SDL_WINDOWEVENT_RESIZED: {
			coord::window new_size{event->window.data1, event->window.data2};

			// call additional handlers for the resize event
			for (auto &handler : this->on_resize) {
				if (false == handler->on_resize(new_size)) {
					break;
				}
			}
			break;
		}
		}
		break;

	case SDL_KEYUP: {
		SDL_Keycode sym = ((SDL_KeyboardEvent *) event)->keysym.sym;
		this->key_states[sym] = false;
	}
		break;

	case SDL_KEYDOWN: {
		SDL_Keycode sym = ((SDL_KeyboardEvent *) event)->keysym.sym;
		this->key_states[sym] = true;
	}
		break;
	}

	return true;
}

void CoreInputHandler::register_resize_action(ResizeHandler *handler) {
	this->on_resize.push_back(handler);
}


} //namespace openage
