// Copyright 2020-2020 the openage authors. See copying.md for legal info.

#ifdef __linux__
#include <gnu/libc-version.h>
#endif
#include <SDL2/SDL.h>
#include <sstream>

#include "versions.h"

using namespace std;

namespace openage
{

map<string,string> get_version_numbers()
{
	map<string,string> version_numbers;

	//Add SDL version number
	SDL_version compiled;
	SDL_VERSION(&compiled)

	string s;
	s.append(to_string(compiled.major));
	s.append(".");
	s.append(to_string(compiled.minor));
	s.append(".");
	s.append(to_string(compiled.patch));

	version_numbers.insert(pair<string,string>("SDL", s));

#ifdef __linux__
//Add libc version number
version_numbers.insert(pair<string,string>("libc",gnu_get_libc_version()));
#endif

	return version_numbers;
}
} // namespace openage
