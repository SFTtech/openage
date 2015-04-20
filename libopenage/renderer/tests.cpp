// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "../log/log.h"
#include "window.h"

namespace openage {
namespace renderer {
namespace tests {


void renderer_demo(int /*argc*/, char **/*argv*/) {
	Window window{"testing!"};
	SDL_Event event;

	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_WINDOWEVENT:
				switch(event.window.event) {
				case SDL_WINDOWEVENT_RESIZED: {
					coord::window new_size{event.window.data1, event.window.data2};
					log::log(
						MSG(info) << "new window size: "
						<< new_size.x << " x " << new_size.y << "."
					);
					break;
				}}
				break;

			case SDL_QUIT:
				running = false;
				break;

			case SDL_KEYUP: {
				SDL_Keycode sym = reinterpret_cast<SDL_KeyboardEvent *>(&event)->keysym.sym;
				switch (sym) {
				case SDLK_ESCAPE:
					running = false;
					break;
				default:
					break;
				}
				break;
			}}
		}

		window.swap();
	}

	return;
}


}}} // namespace openage::renderer::tests
