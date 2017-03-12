// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "window.h"

#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "opengl/context.h"
#include "../log/log.h"
#include "../error/error.h"


namespace openage {
namespace renderer {

class SDL {
public:
	static void make_available() {
		if (!inited) {
			if (SDL_Init(SDL_INIT_VIDEO) < 0) {
				throw Error{MSG(err) << "SDL video initialization failed: " << SDL_GetError()};
			}

			// Initialize support for the PNG image formats, jpg bit: IMG_INIT_JPG
			int wanted_image_formats = IMG_INIT_PNG;
			int sdlimg_inited = IMG_Init(wanted_image_formats);
			if ((sdlimg_inited & wanted_image_formats) != wanted_image_formats) {
				throw Error{MSG(err) << "Failed to initialize PNG support: " << IMG_GetError()};
			}
		}

		sdl = SDL();

		SDL_version version;
		SDL_GetVersion(&version);

		log::log(MSG(info) << "Initialized SDL " << uint32_t(version.major) << "." << uint32_t(version.minor) << "." << uint32_t(version.patch));
	}

	~SDL() {
		if (inited) {
			IMG_Quit();
			SDL_Quit();

			log::log(MSG(info) << "Exited SDL");
		}
	}

private:
	static SDL sdl;
	static bool inited;
};

bool SDL::inited = false;

Window::Window(const char *title)
	: size{800, 600} {
	// TODO: ^ detect screen resolution and determine window size from it.

	SDL::make_available();

	// Even with Vulkan rendering, we still need GL for QtQuick.
	// We need HIGHDPI for eventual support of GUI scaling. (but it requires newer SDL2?)
	int32_t window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED; //| SDL_WINDOW_HIGHDPI;
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

	log::log(MSG(info) << "Created SDL window");

	this->context = opengl::GlContext(this->window);

	// TODO catch window events such as resizes
}

Window::~Window() {
	SDL_DestroyWindow(this->window);
	log::log(MSG(info) << "Destroyed SDL window");
}

void Window::swap() {
	SDL_GL_SwapWindow(this->window);
}

void Window::make_context_current() {
	SDL_GL_MakeCurrent(this->window, this->context->get_raw_context());
}

coord::window Window::get_size() {
	return this->size;
}

void Window::set_size(const coord::window &new_size) {
	if (this->size.x != new_size.x || this->size.y != new_size.y) {
		SDL_SetWindowSize(this->window, this->size.x, this->size.y);
		this->size = new_size;
	}
}

opengl::GlContext* Window::get_context() {
	return &this->context.value();
}

SDL_Window* Window::get_raw_window() const {
	return this->window;
}

}} //openage::renderer
