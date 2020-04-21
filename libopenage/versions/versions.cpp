// Copyright 2020-2020 the openage authors. See copying.md for legal info.

#include "versions.h"

#ifndef __APPLE__
#include <gnu/libc-version.h>
#endif

#include <SDL2/SDL.h>
#include <sstream>

#include "versions/compiletime.h"
#include "../util/strings.h"

namespace openage::versions {

std::map<std::string, std::string> get_version_numbers() {
	std::map<std::string, std::string> version_numbers;

	// https://wiki.libsdl.org/SDL_VERSION
	SDL_version sdl_compiled_version;
	SDL_VERSION(&sdl_compiled_version);
	version_numbers.emplace("SDL", util::sformat("%d.%d.%d",
	                                             sdl_compiled_version.major,
	                                             sdl_compiled_version.minor,
	                                             sdl_compiled_version.patch));


	SDL_version sdl_runtime_version;
	SDL_GetVersion(&sdl_runtime_version);
	version_numbers.emplace("SDL_runtime", util::sformat("%d.%d.%d",
	                                                     sdl_runtime_version.major,
	                                                     sdl_runtime_version.minor,
	                                                     sdl_runtime_version.patch));

#ifdef __APPLE__
	// TODO: i'm too stupid to find a get_version() for the apple libc
	//       please add it here if you know it.
	version_numbers.emplace("libc_runtime", "Apple");
#else
	version_numbers.emplace("libc_runtime", gnu_get_libc_version());
#endif

	version_numbers.emplace("nyan", nyan_version);

	return version_numbers;
}

} // namespace openage::versions
