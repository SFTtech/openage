// Copyright 2020-2020 the openage authors. See copying.md for legal info.

#ifdef __linux__
#include <gnu/libc-version.h>
#endif
#include <SDL2/SDL.h>
#include <sstream>

#include "versions.h"


namespace openage {

std::map<std::string, std::string> get_version_numbers() {
	std::map<std::string, std::string> version_numbers;

	// SDL version
	SDL_version compiled;
	SDL_VERSION(&compiled)

	std::string s;
	s.append(std::to_string(compiled.major));
	s.append(".");
	s.append(std::to_string(compiled.minor));
	s.append(".");
	s.append(std::to_string(compiled.patch));
	version_numbers.emplace("SDL", s);

#ifdef __linux__
	// libc runtime version number
	version_numbers.emplace("libc", gnu_get_libc_version());
#endif

	return version_numbers;
}

} // namespace openage
