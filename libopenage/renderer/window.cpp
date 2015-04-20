// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "window.h"

#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../log/log.h"
#include "../util/error.h"


namespace openage {
namespace renderer {

Window::Window(const char *title) {
	this->context = Context::generate(context_type::autodetect);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw util::Error(MSG(err) << "SDL video initialization: " << SDL_GetError());
	} else {
		log::log(MSG(info) << "Initialized SDL video subsystems.");
	}

	this->context->prepare();

	int32_t window_flags = this->context->get_window_flags() | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
	this->window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		this->window_size.x,
		this->window_size.y,
		window_flags
	);

	if (this->window == nullptr) {
		throw util::Error(MSG(err) << "Failed to create SDL window: " << SDL_GetError());
	} else {
		log::log(MSG(info) << "Created SDL window.");
	}

	this->context->create(this->window);

	// load support for the PNG image formats, jpg bit: IMG_INIT_JPG
	int wanted_image_formats = IMG_INIT_PNG;
	int sdlimg_inited = IMG_Init(wanted_image_formats);
	if ((sdlimg_inited & wanted_image_formats) != wanted_image_formats) {
		throw util::Error(MSG(err) << "Failed to init PNG support: " << IMG_GetError());
	}

	this->context->setup();
}

Window::~Window() {
	log::log(MSG(info) << "Destroying render context...");
	this->context->destroy();

	log::log(MSG(info) << "Destroying window...");
	SDL_DestroyWindow(this->window);

	log::log(MSG(info) << "Exiting SDL...");
	IMG_Quit();
	SDL_Quit();
}

void Window::swap() {
	SDL_GL_SwapWindow(this->window);
}

}} // namespace openage::renderer
