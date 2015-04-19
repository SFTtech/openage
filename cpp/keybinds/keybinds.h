// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_KEYBINDS_KEYBINDS_H_
#define OPENAGE_KEYBINDS_KEYBINDS_H_

#include <functional>
#include <map>
#include <unordered_map>
#include <SDL2/SDL.h>

#include "action.h"
#include "key.h"

namespace openage {
namespace keybinds {

class Keybinds {

public:
    Keybinds();

    void bind(action_t a, std::function<void()> f);
    void press(key_t k);

    std::unordered_map<key_t, action_t, key_hash> keys;
    std::array<std::function<void()>, (std::size_t)action_t::MAX> binds;
};

} //namespace keybinds
} //namespace openage

#endif
