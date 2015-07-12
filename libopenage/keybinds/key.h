// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_KEYBINDS_KEY_H_
#define OPENAGE_KEYBINDS_KEY_H_

#include <SDL2/SDL.h>

namespace openage {
namespace keybinds {

struct key_t {
public:
	key_t(SDL_Keycode key);
	key_t(SDL_Keycode key, SDL_Keymod mod);

	SDL_Keycode key;
	SDL_Keymod mod;
};

bool operator ==(key_t a, key_t b);

struct key_hash {
	int operator()(const key_t &k) const;
};

} //namespace keybinds
} //namespace openage

#endif
