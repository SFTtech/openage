// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include <array>

#include "keybinds.h"

namespace openage {
namespace keybinds {

Keybinds::Keybinds()
    :
    keys { // TODO get this from a file instead of hardcoding it here
        { SDLK_ESCAPE, action_t::STOP_GAME },
        { SDLK_F1, action_t::TOGGLE_HUD },
        { SDLK_F2, action_t::SCREENSHOT },
        { SDLK_F3, action_t::TOGGLE_DEBUG_OVERLAY },
        { SDLK_F4, action_t::TOGGLE_DEBUG_GRID },
        { SDLK_F5, action_t::QUICK_SAVE },
        { SDLK_F9, action_t::QUICK_LOAD },
        { SDLK_SPACE, action_t::TOGGLE_BLENDING },
        { SDLK_F12, action_t::TOGGLE_PROFILER },
        { SDLK_m, action_t::TOGGLE_CONSTRUCT_MODE },
        { SDLK_p, action_t::TOGGLE_UNIT_DEBUG },
        { SDLK_t, action_t::TRAIN_OBJECT },
        { SDLK_y, action_t::ENABLE_BUILDING_PLACEMENT },
        { SDLK_z, action_t::DISABLE_SET_ABILITY },
        { SDLK_x, action_t::SET_ABILITY_MOVE },
        { SDLK_c, action_t::SET_ABILITY_GATHER }
    }
{

}

void Keybinds::bind(action_t a, std::function<void()> f) {
    binds[(size_t)a] = f;
}

void Keybinds::press(SDL_Keycode c) {
    auto f = keys.find(c);
    if (f != keys.end()) {
        binds[(size_t)f->second]();
    }
}

} //namespace keybinds
} //namespace openage
