// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <cstdlib>
#include <epoxy/gl.h>
#include <functional>
#include <unordered_map>

#include "../log/log.h"
#include "window.h"

namespace openage {
namespace renderer {
namespace tests {


struct render_demo {
	std::function<void(Window *)> setup;
	std::function<void()> frame;
	std::function<void(const coord::window &)> resize;
};


void create_window(const render_demo *actions) {
	Window window{"openage renderer testing"};
	SDL_Event event;

	actions->setup(&window);

	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED: {
					coord::window new_size{event.window.data1, event.window.data2};
					log::log(
						MSG(info) << "new window size: "
						<< new_size.x << " x " << new_size.y
					);
					window.set_size(new_size);
					actions->resize(new_size);
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

		actions->frame();

		window.swap();
	}
}


void renderer_demo(int argc, char **argv) {
	int demo_id ;
	if (argc == 2) {
		demo_id = atoi(argv[1]);
		log::log(MSG(info) << "running renderer demo " << demo_id << "...");
	} else {
		demo_id = 0;
		log::log(MSG(info) << "executing default renderer demo " << demo_id << "...");
	}

	render_demo test0{
		// init
		[=](Window */*window*/) {
			glEnable(GL_BLEND);
		},
		// frame
		[] {
			glClearColor(0.0, 0.0, 1.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);

			glColor3f(1, 0, 0);
			glBegin(GL_QUADS); {
				glVertex2f(.0f, .0f);
				glVertex2f(1.0f, .0f);
				glVertex2f(1.0f, 1.0f);
				glVertex2f(.0f, 1.0f);
			} glEnd();
		},
		// resize
		[](const coord::window &new_size) {
			glViewport(0, 0, new_size.x, new_size.y);
		}
	};

	std::unordered_map<int, render_demo*> demos;
	demos[0] = &test0;

	if (demos.find(demo_id) != demos.end()) {
		create_window(demos[demo_id]);
	}
	else {
		log::log(MSG(err) << "unknown renderer demo " << demo_id << " requested.");
	}
}


}}} // namespace openage::renderer::tests
