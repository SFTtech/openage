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

    void set_key_state(SDL_Keycode k, bool is_down);

    /**
     * query stored pressing stat for a key.
     * @return true when the key is pressed, false else.
     */
    bool is_key_down(SDL_Keycode k);

private:

    std::unordered_map<key_t, action_t, key_hash> keys;
    std::array<std::function<void()>, (std::size_t)action_t::MAX> binds;

    /**
     * key to is_down map.
     * stores a mapping between keycodes and its pressing state.
     * a true value means the key is currently pressed,
     * false indicates the key is untouched.
     */
    std::unordered_map<SDL_Keycode, bool> key_states;
};

} //namespace keybinds
} //namespace openage

#endif
