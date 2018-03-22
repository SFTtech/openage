// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#include "window.h"

#include "../../log/log.h"
#include "../../error/error.h"
#include "../sdl_global.h"

#include "renderer.h"


namespace openage {
namespace renderer {
namespace opengl {

GlWindow::GlWindow(const char *title, coord::window size)
	: Window(size)
{
	make_sdl_available();

	// We need HIGHDPI for eventual support of GUI scaling.
	// TODO HIGHDPI fails (requires newer SDL2?)
	int32_t window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
	this->window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		this->size.x,
		this->size.y,
		window_flags
	);

	if (this->window == nullptr) {
		throw Error{MSG(err) << "Failed to create SDL window: " << SDL_GetError()};
	}

	this->context = opengl::GlContext(this->window);
	this->add_resize_callback([] (coord::window new_size) { glViewport(0, 0, new_size.x, new_size.y); } );

	log::log(MSG(info) << "Created SDL window with OpenGL context.");
}

GlWindow::~GlWindow() {
	SDL_DestroyWindow(this->window);
}

void GlWindow::set_size(const coord::window &new_size) {
	if (this->size.x != new_size.x || this->size.y != new_size.y) {
		SDL_SetWindowSize(this->window, this->size.x, this->size.y);
		this->size = new_size;
	}

	for (auto& cb : this->on_resize) {
		cb(new_size);
	}
}

void GlWindow::update() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				coord::window new_size { event.window.data1, event.window.data2 };
				log::log(MSG(dbg) << "Window resized to: " << new_size.x << " x " << new_size.y);
				this->size = new_size;
				for (auto& cb : this->on_resize) {
					cb(new_size);
				}
			}
		} else if (event.type == SDL_QUIT) {
			this->should_be_closed = true;
			// TODO call on_destroy
		} else if (event.type == SDL_KEYUP) {
			auto const ev = *reinterpret_cast<SDL_KeyboardEvent const*>(&event);
			for (auto& cb : this->on_key) {
				cb(ev);
			}
			// TODO handle keydown
		} else if (event.type == SDL_MOUSEBUTTONUP) {
			auto const ev = *reinterpret_cast<SDL_MouseButtonEvent const*>(&event);
			for (auto& cb : this->on_mouse_button) {
				cb(ev);
			}
			// TODO handle mousedown
		}
	}

	SDL_GL_SwapWindow(this->window);
}

std::shared_ptr<Renderer> GlWindow::make_renderer() {
	return std::make_shared<GlRenderer>(this->get_context());
}

void GlWindow::make_context_current() {
	SDL_GL_MakeCurrent(this->window, this->context->get_raw_context());
}

opengl::GlContext *GlWindow::get_context() {
	return &this->context.value();
}

}}} // namespace openage::renderer::opengl
