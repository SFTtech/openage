// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "button.h"

#include <SDL2/SDL_mouse.h>

namespace openage {
namespace gui {

bool Button::mouse_moved_bg(int x, int y) {
	if (state == State::NORMAL) {
		assert(contains(x, y));
		set_state(State::HOVERED);
	}
	else if (state == State::DOWN) {
		if (!contains(x, y)) set_state(State::DOWN_OUT);
	}
	else if (state == State::DOWN_OUT) {
		if (contains(x, y)) set_state(State::DOWN);
	}
	return true;
}

void Button::mouse_left_bg() {
	set_state(State::NORMAL);
}

bool Button::mouse_pressed_bg(std::uint8_t button) {
	if (button == SDL_BUTTON_LEFT) {
		set_state(State::DOWN);
		return true;
	}
	return false;
}

void Button::mouse_released_bg(std::uint8_t button) {
	if (button == SDL_BUTTON_LEFT) {
		//if (state == State::NORMAL) return; // why would this happen?

		if (state == State::DOWN) {
			set_state(State::HOVERED);
			sig_click.emit();
		}
		else {
			assert(state == State::DOWN_OUT);
			set_state(State::NORMAL);
		}
	}
}

void Button::set_state(State new_state) {
	state = new_state;
	sig_state_change.emit(new_state);
}

} // namespace gui
} // namespace openage
