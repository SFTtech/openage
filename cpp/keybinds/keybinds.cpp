// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <array>

#include "keybinds.h"

namespace openage {
namespace keybinds {

Keybinds::Keybinds()
    :
    keys { // TODO get this from a file instead of hardcoding it here
        { key_t(SDLK_ESCAPE), action_t::STOP_GAME },
        { key_t(SDLK_F1), action_t::TOGGLE_HUD },
        { key_t(SDLK_F2), action_t::SCREENSHOT },
        { key_t(SDLK_F3), action_t::TOGGLE_DEBUG_OVERLAY },
        { key_t(SDLK_F4), action_t::TOGGLE_DEBUG_GRID },
        { key_t(SDLK_F5), action_t::QUICK_SAVE },
        { key_t(SDLK_F9), action_t::QUICK_LOAD },
        { key_t(SDLK_SPACE), action_t::TOGGLE_BLENDING },
        { key_t(SDLK_F12), action_t::TOGGLE_PROFILER },
        { key_t(SDLK_m), action_t::TOGGLE_CONSTRUCT_MODE },
        { key_t(SDLK_p), action_t::TOGGLE_UNIT_DEBUG },
        { key_t(SDLK_t), action_t::TRAIN_OBJECT },
        { key_t(SDLK_y), action_t::ENABLE_BUILDING_PLACEMENT },
        { key_t(SDLK_z), action_t::DISABLE_SET_ABILITY },
        { key_t(SDLK_x), action_t::SET_ABILITY_MOVE },
        { key_t(SDLK_c), action_t::SET_ABILITY_GATHER }
    }
{

}

void Keybinds::bind(action_t a, std::function<void()> f) {
    binds[(size_t)a] = f;
}

void Keybinds::press(key_t k) {
    auto f = keys.find(k);
    if (f != keys.end()) {
        binds[(size_t)f->second]();
    }
}

void Keybinds::set_key_state(SDL_Keycode k, bool is_down) {
    key_states[k] = is_down;
}

// not that the function stores a unknown/new keycode
// as 'not pressed' when it was requested
bool Keybinds::is_key_down(SDL_Keycode k) {
    auto it = this->key_states.find(k);
    if (it != this->key_states.end()) {
        return it->second;
    } else {
        this->key_states[k] = false;
        return false;
    }
}

} //namespace keybinds
} //namespace openage
