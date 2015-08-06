// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "key.h"

#include <functional>

namespace openage {
namespace keybinds {

key_t::key_t(SDL_Keycode key)
	:
	key{key},
	mod{KMOD_NONE} {}


key_t::key_t(SDL_Keycode key, SDL_Keymod mod)
	:
	key{key},
	mod{mod} {}


bool operator ==(key_t a, key_t b) {
	return a.key == b.key && a.mod == b.mod;
}


int key_hash::operator()(const key_t &k) const {
	return std::hash<int>()(k.key);
}


} //namespace keybinds
} //namespace openage
