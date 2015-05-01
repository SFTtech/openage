// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "toplevel.h"

namespace openage {
namespace gui {

bool TopLevel::process_event(SDL_Event *event) {
	auto &evt = *event;

	switch(evt.type) {
	case SDL_MOUSEMOTION:
		return mouse_moved(evt.motion.x, evt.motion.y);

	case SDL_MOUSEBUTTONDOWN:
		return mouse_pressed(evt.button.button);

	case SDL_MOUSEBUTTONUP:
		if (mouse_capture_count > 0) {
			mouse_released(evt.button.button);
			return true;
		}
		return false;

	case SDL_MOUSEWHEEL:
		return mouse_scrolled(evt.wheel.x * 100, evt.wheel.y * 100);

	default:
		return false;
	}
}

} // namespace gui
} // namespace openage
