// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#include "window.h"

#include "../../log/log.h"
#include "../../error/error.h"
#include "../sdl_global.h"

#include "renderer.h"


namespace openage {
namespace renderer {
namespace opengl {

GlWindow::GlWindow(const char *title, size_t width, size_t height)
	: Window(width, height)
{
	make_sdl_available();

	// We need HIGHDPI for eventual support of GUI scaling.
	// TODO HIGHDPI fails (requires newer SDL2?)
	int32_t window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
	this->window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		this->size.first,
		this->size.second,
		window_flags
	);

	if (this->window == nullptr) {
		throw Error{MSG(err) << "Failed to create SDL window: " << SDL_GetError()};
	}

	this->context = opengl::GlContext(this->window);
	this->add_resize_callback([] (size_t w, size_t h) { glViewport(0, 0, w, h); } );

	log::log(MSG(info) << "Created SDL window with OpenGL context.");
}

GlWindow::~GlWindow() {
	SDL_DestroyWindow(this->window);
}

void GlWindow::set_size(size_t width, size_t height) {
	if (this->size.first != width || this->size.second != height) {
		SDL_SetWindowSize(this->window, width, height);
		this->size = std::make_pair(width, height);
	}

	for (auto& cb : this->on_resize) {
		cb(width, height);
	}
}

void GlWindow::update() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				size_t width = event.window.data1;
				size_t height = event.window.data2;
				log::log(MSG(dbg) << "Window resized to: " << width << "x" << height);
				this->size = std::make_pair(width, height);
				for (auto& cb : this->on_resize) {
					cb(width, height);
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

std::unique_ptr<Renderer> GlWindow::make_renderer() {
	return std::make_unique<GlRenderer>(this->get_context());
}

void GlWindow::make_context_current() {
	SDL_GL_MakeCurrent(this->window, this->context->get_raw_context());
}

opengl::GlContext *GlWindow::get_context() {
	return &this->context.value();
}

}}} // namespace openage::renderer::opengl
