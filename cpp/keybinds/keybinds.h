// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_KEYBINDS_KEYBINDS_H_
#define OPENAGE_KEYBINDS_KEYBINDS_H_

#include <functional>
#include <map>
#include <unordered_map>
#include <SDL2/SDL.h>

#include "action.h"

namespace openage {
namespace keybinds {

class Keybinds {

public:
    Keybinds();

    void bind(action_t a, std::function<void()> f);
    void press(SDL_Keycode c);

    std::unordered_map<SDL_Keycode, action_t> keys;
    std::array<std::function<void()>, (std::size_t)action_t::MAX> binds;
};

} //namespace keybinds
} //namespace openage

#endif
