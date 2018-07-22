// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../log/log.h"
#include "../error/error.h"


namespace openage {
namespace renderer {

/// A static SDL singleton manager. Used to lazily initialize SDL.
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

		SDL::sdl = SDL();

		SDL_version version;
		SDL_GetVersion(&version);

		log::log(MSG(info) << "Initialized SDL " << uint32_t(version.major) << "." << uint32_t(version.minor) << "." << uint32_t(version.patch));

		SDL::inited = true;
	}

	~SDL() {
		if (SDL::inited) {
			IMG_Quit();
			SDL_Quit();

			log::log(MSG(info) << "Destroyed SDL global context");
		}
	}

private:
	static SDL sdl;
	static bool inited;
};

bool SDL::inited = false;

void make_sdl_available() {
	SDL::make_available();
}

}} // namespace openage::renderer
